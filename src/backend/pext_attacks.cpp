#include <chess/backend/pext_attacks.hpp>

namespace chess {
    const PextAttacks PEXT_ATTACKS;

    PextAttacks::PextAttacks() {
        std::array<Direction, 4> ROOK_DIRECTIONS = {NORTH, EAST, SOUTH, WEST};
        std::array<Direction, 4> BISHOP_DIRECTIONS = {NORTHEAST, NORTHWEST, SOUTHEAST, SOUTHWEST};

        auto offset = init_tables_single(ATTACKS, 0, BISHOP_MASKS, BISHOP_INDICES, BISHOP_DIRECTIONS);
        offset = init_tables_single(ATTACKS, offset, ROOK_MASKS, ROOK_INDICES, ROOK_DIRECTIONS);
    }

    BitBoard PextAttacks::sliding_attacks(Square start, const std::array<Direction, 4>& directions, BitBoard occluders) {
        BitBoard attack;
        for (auto d : directions) {
            BitBoard edges;
            switch (d) {
                case NORTH: edges = BB::R8; break;
                case SOUTH: edges = BB::R1; break;
                case EAST: edges = BB::H; break;
                case WEST: edges = BB::A; break;
                case NORTHEAST: edges = BB::R8 | BB::H; break;
                case NORTHWEST: edges = BB::R8 | BB::A; break;
                case SOUTHEAST: edges = BB::R1 | BB::H; break;
                case SOUTHWEST: edges = BB::R1 | BB::A; break;
                default: assert(false);
            }

            auto square = start;
            while (true) {
                if (!(edges & square).empty()) break;
                if (!(occluders & square).empty()) break;
                square = square.shift(d);
                attack |= square;
            }
        }
        return attack;
    }

    size_t PextAttacks::init_tables_single(std::array<BitBoard, SIZE>& attacks, size_t attacks_offset,
                                          std::array<BitBoard, 64>& masks, std::array<size_t, 64>& indices,
                                          const std::array<Direction, 4>& directions) {
        for (auto from_square : BB::ALL) {
            auto edges = ((BB::R1 | BB::R8) & ~BitBoard(from_square.rank())) |
                         ((BB::A | BB::H) & ~BitBoard(from_square.file()));

            auto mask = sliding_attacks(from_square, directions, 0) & ~edges;

            masks[from_square.get()] = mask;
            indices[from_square.get()] = attacks_offset;

            auto offset_attacks = &attacks[attacks_offset];
            for (auto occluders : mask.subsets()) {
                offset_attacks[utils::pext(occluders.bb, mask.bb)] = sliding_attacks(from_square, directions,
                                                                                     occluders);
                assert(attacks_offset < attacks.size());
                ++attacks_offset;
            }
        }
        return attacks_offset;
    }
}