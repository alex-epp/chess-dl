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
	for (auto i = 0; i < 1'000'000; ++i) {
		b.legal_moves(moves);
		if (moves.empty()) break;
		auto move = moves[rand() % moves.size()];
		b.push_move(move);
	}
	auto end_time = std::chrono::system_clock::now();

	std::wcout << "Took: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " ms." << std::endl;

	std::wcout << b << std::endl;


	//auto b = chess::load_FEN<chess::Board>(chess::STARTING_FEN);

	//auto start_time = std::chrono::system_clock::now();
	//std::wcout << b.perft(3) << std::endl;
	//auto end_time = std::chrono::system_clock::now();

	//std::wcout << "Took: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " ms." << std::endl;

	/*auto b1 = chess::load_FEN<chess::Board>(chess::STARTING_FEN);
	auto b2 = chess::load_FEN<chess::Board>("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
	auto b3 = chess::load_FEN<chess::Board>("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
	auto b4 = chess::load_FEN<chess::Board>("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
	auto b5 = chess::load_FEN<chess::Board>("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 ");
	auto b6 = chess::load_FEN<chess::Board>("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");*/

	//for (auto m : b3.get_moves())
	//	std::wcout << b3.get_piece_at(m.from()).type() << ": " << m.from().get() << ' ' << m.to().get() << std::endl;

	//std::wcout << b1.perft(4) << std::endl;
}
