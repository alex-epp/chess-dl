#include "../eval.hpp"
#include "../random_engine.hpp"

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

    std::cout << engine::eval(engine, opponent, 1) << std::endl;

    return EXIT_SUCCESS;
}