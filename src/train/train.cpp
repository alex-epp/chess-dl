#include <train/datasets/CHESS.hpp>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <torch/torch.h>
#include <filesystem>


using namespace torch::data;
namespace fs = std::filesystem;
using namespace std::chrono;
using chess::datasets::CHESS;


class LinearImpl : public torch::nn::Module {
public:
    LinearImpl(int64_t N, int64_t M)
            : linear(register_module("linear", torch::nn::Linear(N, M)))
    {}

    torch::Tensor forward(torch::Tensor input) {
        return this->linear(input);
    }
private:
    torch::nn::Linear linear;
};
TORCH_MODULE(Linear);


int main(int argc, char** argv) {
    std::vector<fs::path> dataset_search_paths{
        "/mnt/lichess_1", "/mnt/lichess_2",  // For running on cybera
        "/home/alex/src/datasets/lichess_1", "/home/alex/src/datasets/lichess_2",
        // TODO: add other possible paths here
    };

    const size_t epoch_size = 40'357'832;
    const size_t batch_size = 32;

    auto device = torch::kCUDA;

    auto ds = CHESS(dataset_search_paths, CHESS::Split::Debug)
                    .map(transforms::Stack<>());
    auto dl = make_data_loader(std::move(ds), DataLoaderOptions().batch_size(batch_size));

    Linear net(836, 128);
    net->to(device);

    torch::optim::Adam optimizer(net->parameters(), torch::optim::AdamOptions(2e-4));

    const auto start_time = steady_clock::now();
    size_t i = 0;

    for (const auto& batch : *dl) {
        net->zero_grad();
        auto y_hat = net->forward(batch.data.to(device));
        auto loss = torch::mse_loss(y_hat, batch.target.reshape({(size_t)batch_size, -1}).to(device));
        loss.backward();
        optimizer.step();

        if (i % 1000 == 0) {
            std::cout << batch.data.sizes() << "\t|\t" << batch.target.sizes() << "\t|\t";
            std::cout << "Loss: " << loss.mean().item<float>() << "\t|\t";
            auto delta = duration_cast<milliseconds>(steady_clock::now() - start_time);
            auto fps = i / static_cast<float>(delta.count()) * 1000;
            std::cout << "FPS: " << fps * batch_size << "\t[" << 100.f * (i * batch_size) / epoch_size << "%]" << std::endl;
        }

        ++i;
    }

    return EXIT_SUCCESS;
}