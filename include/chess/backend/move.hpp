#pragma once


#include "piece.hpp"
#include "square.hpp"


namespace chess {
	class Move {
	public:
		static constexpr unsigned int QUIET = 0b0000;
		static constexpr unsigned int D_P_PUSH = 0b0001;
		static constexpr unsigned int K_CASTLE = 0b0010;
		static constexpr unsigned int Q_CASTLE = 0b0011;
		static constexpr unsigned int CAPTURE = 0b0100;
		static constexpr unsigned int EN_CAPTURE = 0b0101;
		static constexpr unsigned int PROMOTION = 0b1000;
		static constexpr unsigned int N_PROMOTION = 0b1000;
		static constexpr unsigned int B_PROMOTION = 0b1001;
		static constexpr unsigned int R_PROMOTION = 0b1010;
		static constexpr unsigned int Q_PROMOTION = 0b1011;
		static constexpr unsigned int N_PROMO_CAPTURE = CAPTURE | N_PROMOTION;
		static constexpr unsigned int B_PROMO_CAPTURE = CAPTURE | B_PROMOTION;
		static constexpr unsigned int R_PROMO_CAPTURE = CAPTURE | R_PROMOTION;
		static constexpr unsigned int Q_PROMO_CAPTURE = CAPTURE | Q_PROMOTION;

		constexpr Move(Square f, Square t, unsigned int flags = QUIET, Piece::Type p = Piece::NO_TYPE) noexcept {
			switch (p) {
			case Piece::KNIGHT: flags |= N_PROMOTION; break;
			case Piece::BISHOP: flags |= B_PROMOTION; break;
			case Piece::ROOK: flags |= R_PROMOTION; break;
			case Piece::QUEEN: flags |= Q_PROMOTION; break;
			case Piece::PAWN: assert(false && "Can't promote to pawn"); break;
			case Piece::KING: assert(false && "Can't promote to king"); break;
			case Piece::NO_TYPE: break;
			}
			this->m_data = ((flags & 0xFu) << 12u) | ((f.get() & 0x3Fu) << 6u) | (t.get() & 0x3Fu);
		}
		[[nodiscard]] constexpr Square to() const noexcept { return this->m_data & 0x3Fu; }
		[[nodiscard]] constexpr Square from() const noexcept { return (this->m_data >> 6u) & 0x3Fu; }
		[[nodiscard]] constexpr auto flags() const noexcept { return (this->m_data >> 12u) & 0x3Fu; }

		[[nodiscard]] constexpr bool is_quiet() const noexcept { return this->flags() == QUIET; }
		[[nodiscard]] constexpr bool is_promotion() const noexcept { return this->flags() & PROMOTION; }
		[[nodiscard]] constexpr bool is_capture() const noexcept { return this->flags() & CAPTURE; }
		[[nodiscard]] constexpr bool is_en_capture() const noexcept { return this->flags() == EN_CAPTURE; }
		[[nodiscard]] constexpr bool is_double_pawn_push() const noexcept { return this->flags() == D_P_PUSH; }
		[[nodiscard]] constexpr bool is_castle() const noexcept { return this->is_king_castle() || this->is_queen_castle(); }
		[[nodiscard]] constexpr Piece::Type promoted_type() const noexcept { return PROMOTION_TYPES[this->flags() & 0b11u]; }

		[[nodiscard]] constexpr bool is_king_castle() const noexcept { return this->flags() == K_CASTLE; }
		[[nodiscard]] constexpr bool is_queen_castle() const noexcept { return this->flags() == Q_CASTLE; }

        [[nodiscard]] constexpr bool operator == (const Move& rhs) const noexcept {
			return this->m_data == rhs.m_data;
		}
	private:
		unsigned int m_data = 0;
		static constexpr Piece::Type PROMOTION_TYPES[4] = { Piece::KNIGHT, Piece::BISHOP, Piece::ROOK, Piece::QUEEN };

		friend struct std::hash<Move>;
	};
}
