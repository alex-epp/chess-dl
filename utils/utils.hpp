#pragma once


namespace utils {

    // Example implementation of std::endian from https://en.cppreference.com/w/cpp/types/endian
    enum class endian
    {
#ifdef _WIN32
        little = 0,
    big    = 1,
    native = little
#else
        little = __ORDER_LITTLE_ENDIAN__,
        big    = __ORDER_BIG_ENDIAN__,
        native = __BYTE_ORDER__
#endif
    };

    template <typename T>
    constexpr T byteswap(T bytes) = delete;

    template <>
    constexpr std::uint16_t byteswap(std::uint16_t bytes) {
#ifdef _WIN32
        return _byteswap_uint16(bytes);
#else
        return __builtin_bswap16(bytes);
#endif
    }

    template <>
    constexpr std::uint32_t byteswap(std::uint32_t bytes) {
#ifdef _WIN32
        return _byteswap_uint32(bytes);
#else
        return __builtin_bswap32(bytes);
#endif
    }

    template <>
    constexpr std::uint64_t byteswap(std::uint64_t bytes) {
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
}