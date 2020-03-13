#pragma once


#include "piece.hpp"
#include "square.hpp"

#include <array>
#include <cassert>


namespace chess {
	/*
	Structure that stores a board state as a list of pieces with least-siginificant-file-mapping
	*/
	class Mailbox {
	public:
		[[nodiscard]] constexpr bool is_piece_at(Square square) const {
			return this->piece_mailbox[square.get()].type() != Piece::NO_TYPE;
		}
        [[nodiscard]] constexpr Piece get(Square square) const {
			return this->piece_mailbox[square.get()];
		}
        constexpr void set(Square square, Piece piece) {
			this->piece_mailbox[square.get()] = piece;
		}

        inline void clear(Square square) {
			this->piece_mailbox[square.get()].clear();
		}
        inline void clear() {
			for (auto& p : this->piece_mailbox)
				p.clear();
		}

        inline bool operator == (const Mailbox& rhs) const {
			return this->piece_mailbox == rhs.piece_mailbox;
		}

        inline void flip() {
			for (size_t i = 0; i < this->piece_mailbox.size() / 2; ++i)
				std::swap(this->piece_mailbox[i], this->piece_mailbox[i^56]);
			
			for (auto& p : this->piece_mailbox)
				p.swap_colour();
		}

	private:
		std::array<Piece, 64> piece_mailbox;
	};
}