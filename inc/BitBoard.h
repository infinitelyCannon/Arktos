#pragma once

#include <cassert>
#include "Types.h"

namespace Arktos
{
    inline BitBoard AttackBB[4][64] = {{0}};
    inline BitBoard RookMasks[64] = {0};
    inline BitBoard BishopMasks[64] = {0};
    inline BitBoard RookTable[64][4096] = {{0}};
    inline BitBoard BishopTable[64][1024] = {{0}};

    constexpr BitBoard SquareBB(ESquare const sq)
    {
        assert(is_ok(sq));
        return 1ULL << sq;
    }

    inline ESquare lsb(BitBoard b)
    {
        assert(b);

#if defined(__GNUC__)
        return ESquare(__builtin_ctzll(b)); // GCC, Clang, ICX
#elif defined(_MSC_VER)
        #ifdef _WIN64 // MSVC, WIN64
        unsigned long bit;
        _BitScanForward64(&bit, b);
        return ESquare(bit);
        #else // MSVC, WIN32
        unsigned long bit;

        if (b & 0xffffffff)
        {
            _BitScanForward(&bit, int32_t(b));
            return ESquare(bit);
        }
        else
        {
            _BitScanForward(&bit, int32_t(b >> 32));
            return ESquare(bit + 32);
        }
        #endif
#else
    #error "Compiler not supported"
#endif
    }

    inline ESquare msb(BitBoard b)
    {
        assert(b);

#if defined(__GNUC__) // GCC, CLang, ICX
        return ESquare(63 ^ __builtin_clzll(b));
#elif defined(_MSC_VER)
        #ifdef _WIN64 // MSVC, WIN64
        unsigned long bit;
        _BitScanReverse64(&bit, b);
        return ESquare(bit);
        #else // MSVC, WIN32
        unsigned long bit;

        if (b >> 32)
        {
            _BitScanReverse(&bit, int32_t(b >> 32));
            return ESquare(bit + 32);
        }
        else
        {
            _BitScanReverse(&bit, int32_t(b));
            return ESquare(bit);
        }
        #endif
#else
        #error "Compiler not supported"
#endif
    }
}