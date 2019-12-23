#pragma once


#include "chess.h"

#include <charconv>
#include <sstream>
#include <string>
#include <string_view>


namespace chess {
	constexpr std::string_view STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	template <typename BoardType>
	BoardType load_FEN(std::string_view fen = STARTING_FEN) {
		// TODO: asserts upon failure here are a terrible idea. Replace with something else.

		std::istringstream iss(fen.data());
		std::vector<std::string> tokens{ std::istream_iterator<std::string>{iss},
										 std::istream_iterator<std::string>{} };
		assert(tokens.size() == 6);
		auto board_fen = tokens[0];
		auto turn_fen = tokens[1];
		auto castle_rights_fen = tokens[2];
		auto en_passant_fen = tokens[3];
		auto half_move_fen = tokens[4];
		auto full_move_fen = tokens[5];

		assert(turn_fen == "w" || turn_fen == "b");
		auto turn = (turn_fen == "w") ? Piece::WHITE : Piece::BLACK;
		auto castle_rights = castle_rights_fen;  // TODO: decide on a castle-rights format
		auto en_passant_target = PositionIndex(en_passant_fen);
		unsigned int half_move, full_move;
		auto half_move_parse_result = std::from_chars(half_move_fen.data(), half_move_fen.data() + half_move_fen.length(), half_move);
		auto full_move_parse_result = std::from_chars(full_move_fen.data(), full_move_fen.data() + full_move_fen.length(), full_move);
		assert(half_move_parse_result.ec != std::errc::invalid_argument);
		assert(full_move_parse_result.ec != std::errc::invalid_argument);

		BoardType board(turn, castle_rights, en_passant_target, half_move, full_move);

		auto file = 0, rank = 7;
		for (auto c : board_fen) {
			if (c == '/') {
				--rank;
				file = 0;
			}
			else if (::isdigit(c)) {
				file += c - '0';
			}
			else {
				assert(0 <= file && file < 8 && 0 <= rank && rank < 8);
				board.put_piece(Piece(c), PositionIndex(static_cast<File>(file), static_cast<Rank>(rank)));
				++file;
			}
		}

		return board;
	}
}