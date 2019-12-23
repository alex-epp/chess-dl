#pragma once

#include "chess.h"


#include <iostream>
#include <map>
#include <string_view>


namespace chess {
	std::wostream& print_board(std::wostream& stream, const BaseBoard& board) {
		for (int ri = 7; ri >= 0; --ri) {
			for (int fi = 0; fi < 8; ++fi) {
				auto r = static_cast<Rank>(ri);
				auto f = static_cast<File>(fi);
				auto piece = board.get_piece_at(PositionIndex(f, r));

				stream << piece_repr(piece);
			}
			stream << '\n';
		}
		return stream;
	}
}