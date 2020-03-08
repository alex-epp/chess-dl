#include <chrono>
#include <cstdlib>
#include <iostream>
#include <torch/torch.h>
#include <filesystem>

#include "datasets/CHESS.hpp"

using namespace torch::data;
namespace fs = std::filesystem;
using namespace std::chrono;
using chess::datasets::CHESS;

int main(int argc, char** argv) {
    const size_t epoch_size = 40'357'832;
    const size_t batch_size = 32;

    auto ds = CHESS(std::vector<fs::path>{"/mnt/lichess_1", "/mnt/lichess_2"}, CHESS::Mode::Debug)
                    .map(transforms::Stack<>());
    auto dl = make_data_loader(std::move(ds), DataLoaderOptions().batch_size(batch_size));

    const auto start_time = steady_clock::now();
    size_t i = 0;

    for (const auto& batch : *dl) {
        if (i % 1000 == 0) {
            std::cout << batch.data.sizes() << "\t|\t" << batch.target.sizes() << "\t|\t";

            auto delta = duration_cast<milliseconds>(steady_clock::now() - start_time);
            auto fps = i / static_cast<float>(delta.count()) * 1000;
            std::cout << "FPS: " << fps * batch_size << "\t[" << 100.f * (i * batch_size) / epoch_size << "%]" << std::endl;
        }

        ++i;
    }

    return EXIT_SUCCESS;
}