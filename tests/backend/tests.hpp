#pragma once

#include <catch2/catch.hpp>
#include <sstream>
#include <string>

#include <chess/backend/chess.hpp>

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