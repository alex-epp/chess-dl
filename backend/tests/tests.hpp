#pragma once

#include "../bitboard.hpp"

#include <sstream>
#include <string>

namespace Catch {
    template<>
    struct StringMaker<chess::BitBoard> {
        static std::string convert(chess::BitBoard const& bb) {
            std::ostringstream oss;
            chess::print_bitboard(oss, bb);
            return oss.str();
        }
    };
}