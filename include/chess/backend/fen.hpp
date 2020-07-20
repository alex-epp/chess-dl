#pragma once


#include "chess.hpp"

#include <charconv>
#include <sstream>
#include <string>
#include <string_view>


namespace chess {
	constexpr std::string_view STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    class FENLoadingException : virtual public std::exception {
    protected:
        std::string msg;
    public:
        explicit inline FENLoadingException(std::string msg) noexcept : msg(std::move(msg)) {}
        ~FENLoadingException() noexcept override = default;
        [[nodiscard]] inline const char* what() const noexcept override {
            return msg.c_str();
        }
    };

	template <typename BoardType>
	BoardType load_FEN(std::string_view fen = STARTING_FEN) {
		std::istringstream iss(fen.data());
		std::vector<std::string> tokens{ std::istream_iterator<std::string>{iss},
										 std::istream_iterator<std::string>{} };
		if (tokens.size() != 6)
		    throw FENLoadingException("Invalid number of tokens in FEN");

		auto board_fen = tokens[0];
		auto turn_fen = tokens[1];
		auto castle_rights_fen = tokens[2];
		auto en_passant_fen = tokens[3];
		auto half_move_fen = tokens[4];
		auto full_move_fen = tokens[5];

		if (turn_fen != "w" and turn_fen != "b")
		    throw FENLoadingException("Turn indicator character can only be one of 'w' or 'b'");

		auto turn = (turn_fen == "w") ? Colour::WHITE : Colour::BLACK;
		auto castle_rights = castle_rights_fen;  // TODO: decide on a castle-rights format
		auto en_passant_target = Square(en_passant_fen);

		unsigned int half_move, full_move;
		if (auto result = std::from_chars(half_move_fen.data(), half_move_fen.data() + half_move_fen.length(), half_move);
		        result.ec != std::errc())
		    throw FENLoadingException("Can't parse half-move FEN");
		if (auto result = std::from_chars(full_move_fen.data(), full_move_fen.data() + full_move_fen.length(), full_move);
		        result.ec != std::errc())
		    throw FENLoadingException("Can't parse full-move FEN");

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
			    if (file < 0 || file >= 8 || rank < 0 || rank >= 8) {
                    throw FENLoadingException("Files and ranks must be between 0 and 7");
                }

				board.put_piece(Piece(c), Square(to_file(file), to_rank(rank)));
				++file;
			}
		}

		return board;
	}
}