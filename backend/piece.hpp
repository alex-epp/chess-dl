#pragma once

#include "util.hpp"

#include <cassert>
#include <string_view>

namespace chess {
    enum class Colour {
        WHITE = 0,
        BLACK,
    };

	/*
	Stores a piece's type and colour.
	*/
	class Piece {
	public:
		enum Type {
			PAWN = 2,
			KNIGHT,
			BISHOP,
			ROOK,
			QUEEN,
			KING,
			NO_TYPE
		};

        [[nodiscard]] static constexpr Colour enemy_colour(const Colour c) {
			return static_cast<Colour>(1 - static_cast<size_t>(c));
		}
		template <Colour C>
		[[nodiscard]] static constexpr Colour enemy_colour() {
		    return Piece::enemy_colour(C);
		}

		constexpr Piece() : m_type(NO_TYPE), m_colour(Colour::WHITE) {}
		constexpr Piece(Type t, Colour c) : m_type(t), m_colour(c) {}
		constexpr Piece(char name) : Piece() {
			auto name_lower = util::tolower(name);
			assert(name_lower == 'p' || name_lower == 'n' || name_lower == 'b' || name_lower == 'r' ||
				name_lower == 'q' || name_lower == 'k');
			this->m_colour = util::isupper(name) ? Colour::WHITE : Colour::BLACK;
			switch (name_lower) {
			case 'p': this->m_type = PAWN; break;
			case 'n': this->m_type = KNIGHT; break;
			case 'b': this->m_type = BISHOP; break;
			case 'r': this->m_type = ROOK; break;
			case 'q': this->m_type = QUEEN; break;
			case 'k': this->m_type = KING; break;
			default: this->m_type = NO_TYPE; break;
			}
		}

		[[nodiscard]] constexpr auto type() const { return this->m_type; }
		[[nodiscard]] constexpr auto colour() const { return this->m_colour; }

		constexpr bool operator == (const Piece& rhs) const {
			return (this->type() == NO_TYPE && rhs.type() == NO_TYPE)
				|| (this->colour() == rhs.colour() && this->type() == rhs.type());
		}

		void swap_colour() { this->m_colour = enemy_colour(this->m_colour); }
		void clear() { this->m_type = NO_TYPE; }

	private:
		Type m_type;
		Colour m_colour;
	};

	constexpr std::wstring_view piece_repr(const Piece& piece) {
		if (piece == Piece('P'))
			return L"\u2659";
		else if (piece == Piece('N'))
			return L"\u2658";
		else if (piece == Piece('B'))
			return L"\u2657";
		else if (piece == Piece('R'))
			return L"\u2656";
		else if (piece == Piece('Q'))
			return L"\u2655";
		else if (piece == Piece('K'))
			return L"\u2654";

		else if (piece == Piece('p'))
			return L"\u265F";
		else if (piece == Piece('n'))
			return L"\u265E";
		else if (piece == Piece('b'))
			return L"\u265D";
		else if (piece == Piece('r'))
			return L"\u265C";
		else if (piece == Piece('q'))
			return L"\u265B";
		else if (piece == Piece('k'))
			return L"\u265A";

		else
			return L"\u2610";
	}
}