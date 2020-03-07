#pragma once


#include "piece.hpp"
#include "square.hpp"


namespace chess {
//	class Move;
//	template<> struct std::hash<Move>;

	class Move {
	public:
		static const unsigned int QUIET = 0b0000;
		static const unsigned int D_P_PUSH = 0b0001;
		static const unsigned int K_CASTLE = 0b0010;
		static const unsigned int Q_CASTLE = 0b0011;
		static const unsigned int CAPTURE = 0b0100;
		static const unsigned int EN_CAPTURE = 0b0101;
		static const unsigned int PROMOTION = 0b1000;
		static const unsigned int N_PROMOTION = 0b1000;
		static const unsigned int B_PROMOTION = 0b1001;
		static const unsigned int R_PROMOTION = 0b1010;
		static const unsigned int Q_PROMOTION = 0b1011;
		static const unsigned int N_PROMO_CAPTURE = CAPTURE | N_PROMOTION;
		static const unsigned int B_PROMO_CAPTURE = CAPTURE | B_PROMOTION;
		static const unsigned int R_PROMO_CAPTURE = CAPTURE | R_PROMOTION;
		static const unsigned int Q_PROMO_CAPTURE = CAPTURE | Q_PROMOTION;

		constexpr Move(Square f, Square t, unsigned int flags = QUIET, Piece::Type p = Piece::NO_TYPE) {
			assert(p != Piece::PAWN && p != Piece::KING);

			switch (p) {
			case Piece::KNIGHT: flags |= N_PROMOTION; break;
			case Piece::BISHOP: flags |= B_PROMOTION; break;
			case Piece::ROOK: flags |= R_PROMOTION; break;
			case Piece::QUEEN: flags |= Q_PROMOTION; break;
			}
			this->m_data = ((flags & 0xF) << 12) | ((f.get() & 0x3F) << 6) | (t.get() & 0x3F);
		}
		constexpr Square to() const { return this->m_data & 0x3F; }
		constexpr Square from() const { return (this->m_data >> 6) & 0x3F; }
		constexpr auto flags() const { return (this->m_data >> 12) & 0x3F; }

		constexpr bool is_quiet() const { return this->flags() == QUIET; }
		constexpr bool is_promotion() const { return this->flags() & PROMOTION; }
		constexpr bool is_capture() const { return this->flags() & CAPTURE; }
		constexpr bool is_en_capture() const { return this->flags() == EN_CAPTURE; }
		constexpr bool is_double_pawn_push() const { return this->flags() == D_P_PUSH; }
		constexpr bool is_castle() const { return this->is_king_castle() || this->is_queen_castle(); }
		constexpr Piece::Type promoted_type() const { return PROMOTION_TYPES[this->flags() & 0b11]; }

		constexpr bool is_king_castle() const { return this->flags() == K_CASTLE; }
		constexpr bool is_queen_castle() const { return this->flags() == Q_CASTLE; }

		constexpr bool operator == (const Move& rhs) {
			return this->m_data == rhs.m_data;
		}
	private:
		unsigned int m_data = 0;
		static constexpr Piece::Type PROMOTION_TYPES[4] = { Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN };

		friend struct std::hash<Move>;
	};
}

//namespace std {
//	template<>
//	struct hash<chess::Move> {
//		size_t operator () (const chess::Move& m) const {
//			return hash<int>()(m.m_data);
//		}
//	};
//}