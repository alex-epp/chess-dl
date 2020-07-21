#include <train/datasets/CHESS.hpp>
#include <chess/backend/chess.hpp>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <random>
#include <optional>
#include <utility>

using namespace chess;
using namespace chess::datasets;
namespace fs = std::filesystem;


CHESS::CHESS(const fs::path& root, Split mode)
        : CHESS(std::vector{root}, mode)
{}

CHESS::CHESS(const std::vector<fs::path>& roots, Split split)
        : cur_file_idx(0), game_files(CHESS::get_game_files(roots, split)), split(split),
          file_line_streamer(game_files[cur_file_idx]),
          shuffled_position_move_streamer(file_line_streamer.next_line().value())
{}


std::vector<fs::path> CHESS::get_game_files(const std::vector<fs::path>& roots, CHESS::Split split) {
    const static std::map<CHESS::Split, fs::path> game_files_by_mode = {
            {CHESS::Split::Train, fs::path(__CHESS_HPP_FILEPATH).parent_path() / "meta/train.txt"},
            {CHESS::Split::Valid, fs::path(__CHESS_HPP_FILEPATH).parent_path() / "meta/valid.txt"},
            {CHESS::Split::Test, fs::path(__CHESS_HPP_FILEPATH).parent_path() / "meta/test.txt"},
            {CHESS::Split::Debug, fs::path(__CHESS_HPP_FILEPATH).parent_path() / "meta/debug.txt"},
    };

    std::ifstream in(game_files_by_mode.at(split));
    if (in.fail())
        throw CHESS::Exception("Failed to open file: " +
                               game_files_by_mode.at(split).string());

    std::vector<fs::path> game_files;
    std::string name;
    while (in >> name) {
        auto path = CHESS::find_path_by_name(name, roots);
        if (path)
            game_files.push_back(*path);
        else
            throw CHESS::Exception("Failed to find file: " + name);
    }
    return game_files;
}

std::optional<fs::path> CHESS::find_path_by_name(const std::string& name, const std::vector<fs::path>& roots) {
    for (const auto& root : roots) {
        if (fs::exists(root / name)) return root / name;
    }
    return std::nullopt;
}

CHESS::BatchType CHESS::get_batch(size_t batch_size) {
    std::vector<torch::data::Example<>> batch;
    batch.reserve(batch_size);
    for (size_t i = 0; i < batch_size; ++i) {
        batch.emplace_back(this->get_next_example());
    }
    return batch;
}

torch::optional<size_t> CHESS::size() const {
    return torch::nullopt;
}

CHESS::ExampleType CHESS::get_next_example() {
    auto next_position_move = this->shuffled_position_move_streamer.next_position_move();

    while (!next_position_move.has_value()) {
        auto next_line = this->file_line_streamer.next_line();

        while (!next_line.has_value()) {
            this->cur_file_idx = (this->cur_file_idx + 1) % this->game_files.size();
            this->file_line_streamer = FileLineStreamer(this->game_files[this->cur_file_idx]);
            next_line = this->file_line_streamer.next_line();
        }

        this->shuffled_position_move_streamer = ShuffledPositionMoveStreamer(next_line.value());
        next_position_move = this->shuffled_position_move_streamer.next_position_move();
    }

    auto [position, move] = next_position_move.value();
    return CHESS::position_move_to_example(position, move);
}


/**
 * Each position is given by 6 piece bitboards (64-bit vectors) per colour, plus additional
 * information for: en-passant target (64-bit one-hot vector) and castling rights (4-bit vector).
 * Full-move and half-move clock information is not used. position_move_to_example() assumes it
 * is white's turn.
 *
 * A move is represented by two 64-bit one-hot vectors, from and to.
 *
 * Overall, x's have size [836] and y's have size [2, 64]
 */
CHESS::ExampleType CHESS::position_move_to_example(const chess::Board& position, const chess::Move& move) {
    if (position.get_turn() == Colour::WHITE)
        return position_move_to_example<Colour::WHITE>(position, move);
    else
        return position_move_to_example<Colour::BLACK>(position, move);
}

torch::Tensor CHESS::BB_to_tensor(const BitBoard& bb) {
    auto bb_tensor = torch::zeros(64);
    for (auto p : bb) {
        bb_tensor[p.get()] = 1;
    }
    return bb_tensor;
}

void CHESS::reset() {
    // TODO: implement reset()
}

FileLineStreamer::FileLineStreamer(const fs::path& filename) : file(filename) {
    assert(this->file);
}

torch::optional<std::string> FileLineStreamer::next_line() {
    std::string line;
    if (std::getline(this->file, line))
        return line;
    else
        return torch::nullopt;
}

/**
 * Reads a simplified version of pgn (no comments, branching, etc.). Tokens should be in
 * the format 1. e4 e5 2. Nf3 Nc6 3. Bb5 a6 etc. Theoretically all we need to do is to
 * skip every third token (starting at token 1) and we should be good.
 *
 * Note that each position and move is returned as if white played it (i.e. flipped if it
 * was actually black's turn).
 *
 * TODO: I'm not sure what this function does with edge cases. Need to test it.
 */
std::vector<std::pair<Board, Move>> datasets::read_pgn(const std::string& pgn) {
    std::vector<std::pair<Board, Move>> v;
    std::istringstream iss(pgn);
    std::string counter_or_end, move_san;
    auto board = load_FEN<chess::Board>(STARTING_FEN);
    for(size_t i = 0; !iss.eof(); ++i) {
        if (i % 3 == 0) {
            iss >> counter_or_end;
        } else {
            iss >> move_san;
            if (move_san == "1-0" || move_san == "0-1" || move_san == "1/2-1/2" || move_san == "*")
                continue;

            auto move = board.parse_san(move_san);
            v.emplace_back(board, move);
            board.push_move(move);
        }
    }
    return v;
}

ShuffledPositionMoveStreamer::ShuffledPositionMoveStreamer(const std::string& game_pgn)
            : i(0), game(read_pgn(game_pgn)) {
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(this->game.begin(), this->game.end(), std::default_random_engine(seed));
}

torch::optional<std::pair<Board, Move>> ShuffledPositionMoveStreamer::next_position_move() {
    if (this->i == this->game.size()) return torch::nullopt;
    else return this->game[this->i++];
}
