#pragma once

#include <chess/backend/chess.hpp>

#include <fstream>
#include <filesystem>
#include <optional>
#include <utility>
#include <vector>
#include <torch/data/datasets/base.h>
#include <torch/data/example.h>
#include <torch/types.h>


namespace chess::datasets {
    namespace fs = std::filesystem;

    std::vector<std::pair<Board, Move>> read_pgn(const std::string& pgn);

    class ShuffledPositionMoveStreamer {
    public:
        explicit ShuffledPositionMoveStreamer(const std::string& game);
        torch::optional<std::pair<Board, Move>> next_position_move();

    private:
        size_t i;
        std::vector<std::pair<Board, Move>> game;
    };

    class FileLineStreamer {
    public:
        explicit FileLineStreamer(const fs::path& filename);
        torch::optional<std::string> next_line();

    private:
        std::ifstream file;
    };

    class CHESS : public torch::data::datasets::StreamDataset<CHESS, torch::optional<std::vector<torch::data::Example<>>>> {
    public:
        using ExampleType = torch::data::Example<>;

        enum class Split {
            Train, Valid, Test, Debug
        };

        explicit CHESS(const fs::path& root, Split mode = Split::Train);
        explicit CHESS(const std::vector<fs::path>& roots, Split mode = Split::Train);

        [[nodiscard]]
        BatchType get_batch(size_t batch_size) override;

        [[nodiscard]]
        torch::optional<size_t> size() const override;

        void reset();

    private:
        ExampleType get_next_example();

        size_t cur_file_idx;

        const std::vector<fs::path> game_files;
        const Split mode;

        FileLineStreamer file_line_streamer;
        ShuffledPositionMoveStreamer shuffled_position_move_streamer;

        static std::vector<fs::path> get_game_files(const std::vector<fs::path>& roots, Split split);
        static std::optional<fs::path> find_path_by_name(const std::string& name, const std::vector<fs::path>& roots);
        static ExampleType position_move_to_example(const Board& position, const Move& move);
        static torch::Tensor BB_to_tensor(const BitBoard& bb);

    public:
        class Exception : public std::exception {
        public:
            explicit inline Exception(std::string msg) : msg(std::move(msg)) {}
            ~Exception() noexcept override = default;
            [[nodiscard]] inline const char* what() const noexcept override { return msg.c_str(); }
        private:
            const std::string msg;
        };
    };

    constexpr std::string_view __CHESS_HPP_FILEPATH = __FILE__;
}
