#pragma once


#include <immintrin.h>

namespace utils {

    template <typename T>
    inline T byteswap(T bytes) = delete;
    template <>
    inline std::uint64_t byteswap(std::uint64_t bytes) {
#ifdef _WIN32
        return _byteswap_uint64(bytes);
#else
        return __builtin_bswap64(bytes);
#endif
    }

    static constexpr char tolower(char c) {
        if ('A' <= c && c <= 'Z') return c - 'A' + 'a';
        else return c;
    }
    static constexpr bool isupper(char c) {
        return 'A' <= c && c <= 'Z';
    }

    inline std::uint64_t pext(std::uint64_t val, std::uint64_t mask) {
        return _pext_u64(val, mask);
    }
}