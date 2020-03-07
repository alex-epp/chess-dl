//
// Created by aepp2 on 3/6/2020.
//

#ifndef CHESS_CHESS_HPP
#define CHESS_CHESS_HPP

#include <fstream>
#include <filesystem>
#include <vector>
#include <torch/data/datasets/base.h>
#include <torch/data/example.h>
#include <torch/types.h>

#include "../../backend/chess.h"


namespace chess::datasets {
    namespace fs = std::filesystem;

    Move read_san(const std::string& san, const Board& board);
    std::vector<std::pair<Board, Move>> read_pgn(const std::string& pgn);

    class ShuffledPositionMoveStreamer {
    public:
        ShuffledPositionMoveStreamer(const std::string& game);
        torch::optional<std::pair<Board, Move>> next_position_move();
    };

    class FileLineStreamer {
    public:
        explicit FileLineStreamer(const fs::path& filename);
        torch::optional<std::string> next_line();

    private:
        std::ifstream file;
    };

    class CHESS : public torch::data::datasets::StreamDataset<CHESS> {
    public:
        using Example = torch::data::Example<>;

        enum class Mode {
            Train, Valid, Test
        };

        explicit CHESS(const fs::path& root, Mode mode = Mode::Train);
        explicit CHESS(const std::vector<fs::path>& roots, Mode mode = Mode::Train);

        [[nodiscard]]
        std::vector<Example> get_batch(size_t batch_size) override;

        [[nodiscard]]
        torch::optional<size_t> size() const override;

    private:
        Example get_next_example();

        size_t cur_file_idx;

        const std::vector<fs::path> game_files;
        const Mode mode;

        FileLineStreamer file_line_streamer;
        ShuffledPositionMoveStreamer shuffled_position_move_streamer;

        static std::vector<fs::path> get_game_files(const std::vector<fs::path>& roots, Mode mode);
        static fs::path find_path_by_name(const std::string& name, const std::vector<fs::path>& roots);
        static Example position_move_to_example(const Board& position, const Move& move);
        static torch::Tensor BB_to_tensor(const BitBoard& bb);

        static const std::array<fs::path, 3> game_files_by_mode;
    };
}

#endif //CHESS_CHESS_HPP
