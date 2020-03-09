//
// Created by aepp2 on 3/6/2020.
//

#include "CHESS.hpp"
#include "../../backend/fen.hpp"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <random>
#include <utility>

using namespace chess;
using namespace chess::datasets;
namespace fs = std::filesystem;

const std::array<fs::path, 4> CHESS::game_files_by_mode = {
    fs::path(__FILE__).parent_path() / "meta/train.txt",
    fs::path(__FILE__).parent_path() / "meta/valid.txt",
    fs::path(__FILE__).parent_path() / "meta/test.txt",
    fs::path(__FILE__).parent_path() / "meta/debug.txt",
};

CHESS::CHESS(const fs::path& root, Mode mode)
        : CHESS(std::vector{root}, mode)
{}

CHESS::CHESS(const std::vector<fs::path>& roots, Mode mode)
        : cur_file_idx(0), game_files(CHESS::get_game_files(roots, mode)), mode(mode),
          file_line_streamer(game_files[cur_file_idx]),
          shuffled_position_move_streamer(file_line_streamer.next_line().value())
{}


std::vector<fs::path> CHESS::get_game_files(const std::vector<fs::path>& roots, CHESS::Mode mode) {
    std::ifstream in(CHESS::game_files_by_mode[static_cast<size_t>(mode)]);
    assert(!in.fail());

    std::vector<fs::path> game_files;
    std::string name;
    while (in >> name) {
        game_files.push_back(CHESS::find_path_by_name(name, roots));
    }
    return game_files;
}

fs::path CHESS::find_path_by_name(const std::string& name, const std::vector<fs::path>& roots) {
    for (const auto& root : roots) {
        if (fs::exists(root / name)) return root / name;
    }
    assert(false && "Cannot find path by name");
    return fs::path();
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
 */
CHESS::ExampleType CHESS::position_move_to_example(const chess::Board& position, const chess::Move& move) {
    assert(position.is_piece_at(move.from()));
    assert(position.get_piece_at(move.from()).colour() == Piece::WHITE);

    auto position_encoded = torch::cat(
            {
                CHESS::BB_to_tensor(position.pawns(Piece::WHITE)),
                CHESS::BB_to_tensor(position.bishops(Piece::WHITE)),
                CHESS::BB_to_tensor(position.knights(Piece::WHITE)),
                CHESS::BB_to_tensor(position.rooks(Piece::WHITE)),
                CHESS::BB_to_tensor(position.queens(Piece::WHITE)),
                CHESS::BB_to_tensor(position.kings(Piece::WHITE)),

                CHESS::BB_to_tensor(position.pawns(Piece::BLACK)),
                CHESS::BB_to_tensor(position.bishops(Piece::BLACK)),
                CHESS::BB_to_tensor(position.knights(Piece::BLACK)),
                CHESS::BB_to_tensor(position.rooks(Piece::BLACK)),
                CHESS::BB_to_tensor(position.queens(Piece::BLACK)),
                CHESS::BB_to_tensor(position.kings(Piece::BLACK)),

                CHESS::BB_to_tensor(position.en_target()),

                torch::tensor({
                    position.white_king_castling_rights(),
                    position.white_queen_castling_rights(),
                    position.black_king_castling_rights(),
                    position.black_queen_castling_rights()
                }, torch::dtype(torch::kFloat32))
            });

    auto move_encoded = torch::stack(
            {
               CHESS::BB_to_tensor(move.from()),
               CHESS::BB_to_tensor(move.to())
            });

    return CHESS::ExampleType(position_encoded, move_encoded);
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
    auto turn = Piece::WHITE;
    for(size_t i = 0; !iss.eof(); ++i) {
        if (i % 3 == 0) {
            iss >> counter_or_end;
        } else {
            iss >> move_san;
            if (move_san == "1-0" || move_san == "0-1" || move_san == "1/2-1/2" || move_san == "*")
                continue;

            auto move = board.parse_san(move_san, turn);
            v.emplace_back(board, move);
            board.push_move(move);
            board.flip();
            turn = Piece::enemy_colour(turn);
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
