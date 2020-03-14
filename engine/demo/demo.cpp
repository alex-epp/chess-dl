#include "engine/eval.hpp"
#include "engine/random_engine.hpp"

#include <random>
#include <iostream>

using namespace chess;
using namespace engine;


std::ostream& operator << (std::ostream& o, const EvalResult& e) {
    return o << "EvalResult (wins=" << e.wins << ", losses=" << e.losses << ", draws=" << e.draws << ")";
}


int main(int argc, char** argv) {
    std::mt19937 random(0);
    auto engine = make_random_engine(random);
    auto opponent = make_random_engine(random);

    auto start_time = std::chrono::system_clock::now();
    auto result = engine::eval(engine, opponent, 100);
    auto end_time = std::chrono::system_clock::now();

    std::cout << result << std::endl;
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Took: " << duration.count() << " ms." << std::endl;

    return EXIT_SUCCESS;
}