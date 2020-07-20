#pragma once


namespace chess {
	enum class File {
		A = 0, B, C, D, E, F, G, H
	};
	enum class Rank {
		R1 = 0, R2, R3, R4, R5, R6, R7, R8
	};

	template <typename T = unsigned int>
	[[nodiscard]] constexpr T to_integral(File f) noexcept {
	    return static_cast<T>(f);
	}

    template <typename T = unsigned int>
    [[nodiscard]] constexpr T to_integral(Rank r) noexcept {
        return static_cast<T>(r);
    }

    template <typename T = unsigned int>
    [[nodiscard]] constexpr Rank to_rank(T r) noexcept {
        assert(0 <= r && r < 8);
        return static_cast<Rank>(r);
    }

    template <typename T = unsigned int>
    [[nodiscard]] constexpr File to_file(T f) noexcept {
        assert(0 <= f && f < 8);
        return static_cast<File>(f);
    }

    template <Colour C>
    [[nodiscard]] constexpr Rank orient(Rank r) noexcept = delete;
    template <>
    [[nodiscard]] constexpr Rank orient<Colour::WHITE> (Rank r) noexcept {
        return r;
    }
    template <>
    [[nodiscard]] constexpr Rank orient<Colour::BLACK> (Rank r) noexcept {
        return to_rank(7 - to_integral(r));
    }

}