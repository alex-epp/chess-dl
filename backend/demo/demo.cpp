// CppChessLib.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <iostream>
#include <chrono>

#include "backend/chess.hpp"

#ifdef _WIN32
    #include <Windows.h>
    #include <io.h>

    void enable_unicode() {
        _setmode(_fileno(stdout), _O_U16TEXT);
    }
#else
    void enable_unicode() {}
#endif


int main()
{
	enable_unicode();

	auto b = chess::load_FEN<chess::Board>(chess::STARTING_FEN);
	std::vector<chess::Move> moves;
	moves.reserve(100);

	auto start_time = std::chrono::system_clock::now();
	size_t i;
	for (i = 0; i < 1'000'000; ++i) {
		b.legal_moves(moves);
		if (moves.empty()) break;
		auto move = moves[rand() % moves.size()];
		b.push_move(move);
	}
	auto end_time = std::chrono::system_clock::now();
	auto duration = end_time - start_time;
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
	std::cout << "Took: " << ms << " ms [ " << ns / i << " ns/move]." << std::endl;
	std::cout << b.fen() << std::endl << b;
}
