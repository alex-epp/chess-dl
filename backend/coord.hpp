#pragma once


namespace chess {
	enum class File {
		A = 0, B, C, D, E, F, G, H
	};
	enum class Rank {
		R1 = 0, R2, R3, R4, R5, R6, R7, R8
	};

    template <Colour C>
    constexpr Rank orient(Rank r) = delete;
    template <>
    constexpr Rank orient<Colour::WHITE> (Rank r) {
        return r;
    }
    template <>
    constexpr Rank orient<Colour::BLACK> (Rank r) {
        return static_cast<Rank>(7 - static_cast<size_t>(r));
    }

}