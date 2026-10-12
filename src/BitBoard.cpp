#include "BitBoard.h"
#include <initializer_list>

namespace Arktos
{
    BitBoard PseudoAttacks[ALL_PIECES][64] = {{0}};
    BitBoard RookMasks[64] = {0};
    BitBoard BishopMasks[64] = {0};
    BitBoard RookTable[64][4096] = {{0}};
    BitBoard BishopTable[64][1024] = {{0}};
    BitBoard Rays[8][64] = {{0}};
    BitBoard LineBB[64][64] = {{0}};
    BitBoard BetweenBB[64][64] = {{0}};

    BitBoard GetBlockersFromIndex(int index, BitBoard mask)
    {
        BitBoard blockers = 0;
        int bits = popCount(mask);

        for (int i = 0; i < bits; ++i)
        {
            int bitPos = popLsb(mask);
            if (index & (1 << i))
            {
                blockers |= (1ULL << bitPos);
            }
        }
        
        return blockers;
    }

    void InitAttacks()
    {
        // Init pawn attacks
        for (int c = COLOR_White; c < COLOR_None; ++c)
        {
            for (int sq = SQ_A1; sq < SQ_ER; ++sq)
            {
                BitBoard square = 1ULL << sq;
                BitBoard east = c == COLOR_White ? square << 9 & ~FILE_A : square >> 7 & ~FILE_A;
                BitBoard west = c == COLOR_White ? square << 7 & ~FILE_H : square >> 9 & ~FILE_H;
                PseudoAttacks[c][sq] = east | west;
            }
        }

        // Init the rest
        for (int sq = SQ_A1; sq < SQ_ER; ++sq)
        {
            BitBoard square = 1ULL << sq;
            BitBoard attacks = square << 1 & ~FILE_A | square >> 1 & ~FILE_H;
            attacks |= square;
            attacks |= attacks << 8 | attacks >> 8;
            PseudoAttacks[KING][sq] = attacks ^ square;

            BitBoard east, west;
            east = square << 1 & ~FILE_A;
            west = square >> 1 & ~FILE_H;
            attacks = (east | west) << 16;
            attacks |= (east | west) >> 16;
            east = east << 1 & ~FILE_A;
            west = west >> 1 & ~FILE_H;
            attacks |= (east | west) << 8;
            attacks |= (east | west) >> 8;
            PseudoAttacks[KNIGHT][sq] = attacks;

            PseudoAttacks[ROOK][sq] = getRay(DIR_North, sq) | getRay(DIR_East, sq)
                | getRay(DIR_South, sq) | getRay(DIR_West, sq);
            
            PseudoAttacks[BISHOP][sq] = getRay(DIR_NorthEast, sq) | getRay(DIR_NorthWest, sq)
                | getRay(DIR_SouthEast, sq) | getRay(DIR_SouthWest, sq);
            
            PseudoAttacks[QUEEN][sq] = PseudoAttacks[ROOK][sq] | PseudoAttacks[BISHOP][sq];
        }

        for (ESquare s1 = SQ_A1; s1 <= SQ_H8; ++s1)
        {
            for (PieceType type : {BISHOP, ROOK})
            {
                for (ESquare s2 = SQ_A1; s2 <= SQ_H8; ++s2)
                {
                    if (PseudoAttacks[type][s1] & s2)
                    {
                        LineBB[s1][s2] = (AttacksBB(type, s1, 0) & AttacksBB(type, s2, 0)) | s1 | s2;
                        BetweenBB[s1][s2] = (AttacksBB(type, s1, SquareBB(s2)) & AttacksBB(type, s2, SquareBB(s1)));
                    }
                    BetweenBB[s1][s2] |= s2;
                }
            }
        }
    }

    BitBoard GetBishopAttacks(ESquare sq, BitBoard blockers)
    {
        // Mask blockers to only include relevant bits
        blockers &= BishopMasks[sq];

        // Generate key using the magic number and bit shift
        uint64_t key = (blockers * BishopMagics[sq]) >> (64 - BishopIndexBits[sq]);

        return BishopTable[sq][key];
    }

    BitBoard GetRookAttacks(ESquare sq, BitBoard blockers)
    {
        // Mask blockers to only include relevant bits
        blockers &= RookMasks[sq];

        // Generate key using the magic number and bit shift
        uint64_t key = (blockers * RookMagics[sq]) >> (64 - RookIndexBits[sq]);

        return RookTable[sq][key];
    }

    void InitBishopMasks()
    {
        BitBoard edgeSquares = FILE_A | FILE_H | RANK_1 | RANK_8;
        for (int square = SQ_A1; square < SQ_ER; ++square)
        {
            BishopMasks[square] = (getRay(DIR_NorthEast, square) | getRay(DIR_NorthWest, square) |
                getRay(DIR_SouthEast, square) | getRay(DIR_SouthWest, square)) & ~edgeSquares;
        }
    }

    void InitRookMasks()
    {
        for (int square = SQ_A1; square < SQ_ER; ++square)
        {
            RookMasks[square] = (getRay(DIR_North, square) & ~RANK_8) |
                (getRay(DIR_South, square) & ~RANK_1) |
                (getRay(DIR_East, square) & ~FILE_H) |
                (getRay(DIR_West, square) & ~FILE_A);
        }
    }

    void InitBishopMagicTable()
    {
        for (int square = SQ_A1; square < SQ_ER; ++square)
        {
            for (int blockerIndex = 0; blockerIndex < (1 << BishopIndexBits[square]); ++blockerIndex)
            {
                BitBoard blockers = GetBlockersFromIndex(blockerIndex, BishopMasks[square]);
                BishopTable[square][(blockers * BishopMagics[square]) >> (64 - BishopIndexBits[square])] =
                    GetBishopAttacksSlow(square, blockers);
            }
        }
    }

    void InitRookMagicTable()
    {
        for (int square = SQ_A1; square < SQ_ER; ++square)
        {
            for (int blockerIndex = 0; blockerIndex < (1 << RookIndexBits[square]); ++blockerIndex)
            {
                BitBoard blockers = GetBlockersFromIndex(blockerIndex, RookMasks[square]);
                RookTable[square][(blockers * RookMagics[square]) >> (64 - RookIndexBits[square])] = 
                    GetRookAttacksSlow(square, blockers);
            }
        }
    }

    BitBoard GetRookAttacksSlow(int square, BitBoard blockers)
    {
        BitBoard attacks = 0;

        attacks |= getRay(DIR_North, square);
        if (getRay(DIR_North, square) & blockers)
        {
            attacks &= ~(getRay(DIR_North, lsb(getRay(DIR_North, square) & blockers)));
        }

        attacks |= getRay(DIR_South, square);
        if (getRay(DIR_South, square) & blockers)
        {
            attacks &= ~(getRay(DIR_South, msb(getRay(DIR_South, square) & blockers)));
        }

        attacks |= getRay(DIR_East, square);
        if (getRay(DIR_East, square) & blockers)
        {
            attacks &= ~(getRay(DIR_East, lsb(getRay(DIR_East, square) & blockers)));
        }

        attacks |= getRay(DIR_West, square);
        if (getRay(DIR_West, square) & blockers)
        {
            attacks &= ~(getRay(DIR_West, msb(getRay(DIR_West, square) & blockers)));
        }

        return attacks;
    }

    BitBoard GetBishopAttacksSlow(int square, BitBoard blockers)
    {
        BitBoard attacks = 0;

        attacks |= getRay(DIR_NorthWest, square);
        if (getRay(DIR_NorthWest, square) & blockers)
        {
            attacks &= ~(getRay(DIR_NorthWest, lsb(getRay(DIR_NorthWest, square) & blockers)));
        }

        attacks |= getRay(DIR_NorthEast, square);
        if (getRay(DIR_NorthEast, square) & blockers)
        {
            attacks &= ~(getRay(DIR_NorthEast, lsb(getRay(DIR_NorthEast, square) & blockers)));
        }

        attacks |= getRay(DIR_SouthEast, square);
        if (getRay(DIR_SouthEast, square) & blockers)
        {
            attacks &= ~(getRay(DIR_SouthEast, msb(getRay(DIR_SouthEast, square) & blockers)));
        }

        attacks |= getRay(DIR_SouthWest, square);
        if (getRay(DIR_SouthWest, square) & blockers)
        {
            attacks &= ~(getRay(DIR_SouthWest, msb(getRay(DIR_SouthWest, square) & blockers)));
        }

        return attacks;
    }

    void InitRays()
    {
        for (unsigned int square = SQ_A1; square < SQ_ER; ++square)
        {
            ESquare sq = static_cast<ESquare>(square);
            Rays[DIR_North][square] = 0x0101010101010100ULL << square;
            Rays[DIR_South][square] = 0x0080808080808080ULL >> (63 - square);
            Rays[DIR_East][square] = 2 * ((1ULL << (square | 7)) - (1ULL << square));
            Rays[DIR_West][square] = (1ULL << square) - (1ULL << (square & 56));
            Rays[DIR_NorthWest][square] = shiftWest(0x102040810204000ULL, 7 - GetFile(sq)) << (GetRank(sq) * 8);
            Rays[DIR_NorthEast][square] = shiftEast(0x8040201008040200ULL, GetFile(sq)) << (GetRank(sq) * 8);
            Rays[DIR_SouthWest][square] = shiftWest(0x40201008040201ULL, 7 - GetFile(sq)) >> ((7 - GetRank(sq)) * 8);
            Rays[DIR_SouthEast][square] = shiftEast(0x2040810204080ULL, GetFile(sq)) >> ((7 - GetRank(sq)) * 8);
        }
    }

    BitBoard getRay(EDirection dir, int square)
    {
        return Rays[dir][square];
    }
}