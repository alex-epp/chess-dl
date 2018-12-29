#include "chess.h"

namespace chess {
	const std::string BaseBoard::STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	const Piece::Type Move::PROMOTION_TYPES[4] = {
		Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN
	};
}
