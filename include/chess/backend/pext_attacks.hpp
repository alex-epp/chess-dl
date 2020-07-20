#pragma once

#include <array>
#include "bitboard.hpp"
#include "square.hpp"
#include <chess/utils/utils.hpp>

namespace chess {

    // TODO: in principle the tables could be calculated at compile time

    class PextAttacks {
    public:
        PextAttacks();

        [[nodiscard]] inline BitBoard rook_attacks(Square from, BitBoard occluders) const noexcept;
        [[nodiscard]] inline BitBoard bishop_attacks(Square from, BitBoard occluders) const noexcept;

    private:
        constexpr static size_t SIZE = 107'648;

        static BitBoard sliding_attacks(Square start,
                                        const std::array<Direction, 4>& directions,
                                        BitBoard occluders) noexcept;

        static size_t init_tables_single(std::array<BitBoard, SIZE>& attacks,
                                         size_t attacks_offset,
                                         std::array<BitBoard, 64>& masks,
                                         std::array<size_t, 64>& indices,
                                         const std::array<Direction, 4>& directions);

        std::array<BitBoard, SIZE> ATTACKS;
        std::array<BitBoard, 64> ROOK_MASKS;
        std::array<BitBoard, 64> BISHOP_MASKS;
        std::array<size_t, 64> ROOK_INDICES;
        std::array<size_t, 64> BISHOP_INDICES;
    };

    inline BitBoard PextAttacks::rook_attacks(Square from, BitBoard occluders) const noexcept {
        return ATTACKS[ROOK_INDICES[from.get()] +
                       utils::pext(occluders.bb, ROOK_MASKS[from.get()].bb)];
    }

    inline BitBoard PextAttacks::bishop_attacks(Square from, BitBoard occluders) const noexcept {
        return ATTACKS[BISHOP_INDICES[from.get()] +
                       utils::pext(occluders.bb, BISHOP_MASKS[from.get()].bb)];
    }

    extern const PextAttacks PEXT_ATTACKS;
}