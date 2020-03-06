#include <cstdlib>
#include <iostream>
#include <torch/torch.h>


int main(int argc, char** argv) {
    torch::Tensor tensor = torch::eye(3).cuda();
    std::cout << tensor << std::endl;

    return EXIT_SUCCESS;
}