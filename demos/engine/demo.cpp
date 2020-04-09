#include <chess/engine/eval.hpp>
#include <chess/engine/random_engine.hpp>

#include <random>
#include <iostream>

using namespace chess;
using namespace engine;


std::ostream& operator << (std::ostream& o, const EvalResult& e) {
    return o << "EvalResult (wins=" << e.wins << ", losses=" << e.losses << ", draws=" << e.draws << ")";
}


int main(int argc, char** argv) {
    std::mt19937 random(0);
    auto opening_book = std::make_shared<MMappedOpeningBook>("../../../assets/gm2001.bin");
    auto engine = make_random_engine(random, opening_book);
    auto opponent = make_random_engine(random, opening_book);

    const size_t trials = 100;

    auto start_time = std::chrono::system_clock::now();
    auto result = engine::eval(engine, opponent, trials);
    auto end_time = std::chrono::system_clock::now();

    std::cout << result << std::endl;
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Took: " << duration.count() / 1000 << " ms [" << duration.count() / trials << " us/trial]." << std::endl;

    return EXIT_SUCCESS;
}