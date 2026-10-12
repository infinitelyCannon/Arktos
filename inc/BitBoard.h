#pragma once

#include "Defs.h"
#include <cassert>
#if defined(_MSC_VER)
#include <intrin.h>
#endif

namespace Arktos
{
    extern BitBoard PseudoAttacks[ALL_PIECES][64];
    extern BitBoard RookMasks[64];
    extern BitBoard BishopMasks[64];
    extern BitBoard RookTable[64][4096];
    extern BitBoard BishopTable[64][1024];
    extern BitBoard Rays[8][64];
    extern BitBoard LineBB[64][64];
    extern BitBoard BetweenBB[64][64];

    // Magic values for Rook and Bishop hash tables
    const uint64_t RookMagics[64] = {
        756607761056301088UL, 486389076241424385UL, 72092778679042112UL, 2630106649158748161UL, 180146733941000192UL, 648589814731636770UL, 180146184134856832UL, 2918332697065554048UL,
        11529355785708503092UL, 1267187687899136UL, 2490631468938694656UL, 293015506591416352UL, 288934097953489024UL, 3096241940480520UL, 2596606664451031552UL, 281616710647938UL,
        9719133065330688UL, 1193595738806829072UL, 576745526890856466UL, 360429257568030720UL, 722829389795952768UL, 9223513324166054400UL, 144397214955930112UL, 576744426307850385UL,
        140739637952517UL, 1170971088563208264UL, 19799803437186UL, 942527774937055360UL, 5070958365245568UL, 72061994232053888UL, 10385301842383864320UL, 581848436590281801UL,
        329150075794096256UL, 72128031505784833UL, 5197155070050832448UL, 288300890958333970UL, 146649081425430530UL, 581105115214119936UL, 4828149286694814032UL, 2323899487900144641UL,
        18049857767899136UL, 9223654062661107744UL, 9516248984046403600UL, 1153765998257045536UL, 577591050391027840UL, 5066584209096708UL, 1152939105383088648UL, 4827859488876920833UL,
        180144604161573632UL, 63067988042981952UL, 35188671256320UL, 2469116105206038656UL, 2251817001949568UL, 562984581857792UL, 9227893237937669120UL, 2305843628831998464UL,
        4647715366276964609UL, 18014538100186369UL, 2307340820539246849UL, 360850989000950818UL, 5231493951774524418UL, 281767035013651UL, 4611687186792841348UL, 10696191184830530UL
    };

    const uint64_t BishopMagics[64] = {
        9917227751251903040UL, 2307114353926610960UL, 2335125204253151232UL, 7099935816794769416UL, 1130435459416064UL, 72200668527460369UL, 13836258868546633738UL, 11400320691298305UL,
        288310780225421568UL, 144612171694473348UL, 37163495452180496UL, 8935681818720UL, 2305988166290530304UL, 3459059462248091652UL, 1155173665466746882UL, 9314574473536473344UL,
        146375853238993920UL, 20266336064686592UL, 1087641317556690952UL, 1154047576346992641UL, 594756660686751761UL, 18049591482261512UL, 140738897911808UL, 9800118663272763392UL,
        369299586550874768UL, 1154051803924398209UL, 2306414859195514913UL, 1126724741955906UL, 10670162350880858112UL, 457257199186501649UL, 110345957249649152UL, 5188217414386523136UL,
        73258399252365824UL, 365081849510826017UL, 1128253566878720UL, 9277419647610126624UL, 9241387535958149122UL, 324401040235561104UL, 72621153876968002UL, 578994379315020032UL,
        9223937469837230208UL, 302023234194513920UL, 7036977531065344UL, 144116571189805184UL, 3677198029416300800UL, 292751911566753824UL, 4616754818640315144UL, 2342470636376440960UL,
        76847071520890880UL, 7494838608309258752UL, 2378182361733988352UL, 9367487226022232098UL, 2269924327424UL, 634426866335808UL, 18032007883821056UL, 586040256495747604UL,
        18155688169644036UL, 40816225332369609UL, 288935455733403656UL, 40533771111638032UL, 281476087169285UL, 4504184552499712UL, 2675147045886527752UL, 4616224805122081088UL
    };

/**
 * @name Number of bits in the magic bitboard table index for rooks
 * and bishops
 * @brief Indexed by [square] with each value being the number of
 * bits required for that square
 */
    const int RookIndexBits[64] = {
        12, 11, 11, 11, 11, 11, 11, 12,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        12, 11, 11, 11, 11, 11, 11, 12
    };

    const int BishopIndexBits[64] = {
        6, 5, 5, 5, 5, 5, 5, 6,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 5, 5, 5, 5, 5, 5, 6
    };

    void InitAttacks();

    void InitRookMasks();
    void InitBishopMasks();
    void InitRookMagicTable();
    void InitBishopMagicTable();

    BitBoard GetRookAttacks(ESquare sq, BitBoard blockers);
    BitBoard GetBishopAttacks(ESquare sq, BitBoard blockers);
    BitBoard GetBlockersFromIndex(int index, BitBoard mask);
    BitBoard GetRookAttacksSlow(int square, BitBoard blockers);
    BitBoard GetBishopAttacksSlow(int square, BitBoard blockers);

    constexpr bool MoreThanOne(BitBoard b) {return b & (b - 1);}

    // Get the spaces between s1 and s2 if it exists (s2 included)
    inline BitBoard GetBetween(ESquare s1, ESquare s2) 
    {
        assert(is_ok(s1) && is_ok(s2));
        return BetweenBB[s1][s2];
    }

    // Get the line of squares from s1 to s2 (inclusive) if it exists
    inline BitBoard GetLine(ESquare s1, ESquare s2)
    {
        assert(is_ok(s1) && is_ok(s2));
        return LineBB[s1][s2];
    }

    // Rays
    void InitRays();
    BitBoard getRay(EDirection dir, int square);
    // end Rays

    inline BitBoard shiftEast(BitBoard board, int n)
    {
        BitBoard newBoard = board;
        for (int i = 0; i < n; ++i)
        {
            newBoard = ((newBoard << 1) & (~FILE_A));
        }

        return newBoard;
    }
    
    inline BitBoard shiftWest(BitBoard board, int n)
    {
        BitBoard newBoard = board;
        for (int i = 0; i < n; ++i)
        {
            newBoard = ((newBoard >> 1) & ~FILE_H);
        }

        return newBoard;
    }

    inline int GetRank(ESquare sq)
    {
        assert(is_ok(sq));
        return sq / 8;
    }

    inline int GetFile(ESquare sq)
    {
        assert(is_ok(sq));
        return sq % 8;
    }

    inline int popCount(BitBoard board)
    {
#if defined(__GNUC__)
        return __builtin_popcountll(board);
#elif defined(_MSC_VER)
        return __popcnt64(board);
#else
        #error "Compiler not supported"
#endif
    }

    constexpr BitBoard SquareBB(ESquare const sq)
    {
        assert(is_ok(sq));
        return 1ULL << sq;
    }

    // Returns the attacks by the given piece
    // assuming the board is occupied according to the passed Bitboard.
    // Sliding piece attacks do not continue past an occupied square.
    template <PieceType Pt>
    inline BitBoard AttacksBB(ESquare sq, EColor c = COLOR_None)
    {
        assert((Pt != PAWN || c < COLOR_None) && is_ok(sq));
        return Pt == PAWN ? PseudoAttacks[c][sq] : PseudoAttacks[Pt][sq];
    }

    // Returns the attacks by the given piece
    // assuming the board is occupied according to the passed Bitboard.
    // Sliding piece attacks do not continue past an occupied square.
    template <PieceType Pt>
    inline BitBoard AttacksBB(ESquare sq, BitBoard occupied)
    {
        assert(Pt != PAWN && is_ok(sq));

        switch (Pt)
        {
        case BISHOP:
            return GetBishopAttacks(sq, occupied);
        case ROOK:
            return GetRookAttacks(sq, occupied);
        case QUEEN:
            return GetBishopAttacks(sq, occupied) | GetRookAttacks(sq, occupied);
        default:
            return PseudoAttacks[Pt][sq];
        }
    }

    // Returns the attacks by the given piece
    // assuming the board is occupied according to the passed Bitboard.
    // Sliding piece attacks do not continue past an occupied square.
    inline BitBoard AttacksBB(PieceType pt, ESquare sq, BitBoard occupied)
    {
        assert(pt != PAWN && is_ok(sq));

        switch(pt)
        {
            case BISHOP:
                return AttacksBB<BISHOP>(sq, occupied);
            case ROOK:
                return AttacksBB<ROOK>(sq, occupied);
            case QUEEN:
                return AttacksBB<QUEEN>(sq, occupied);
            default:
                return PseudoAttacks[pt][sq];
        }
    }

    // Count trailing zeros
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

    // Count leading zeros
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

    // Removes and returns the least significant bit
    inline ESquare popLsb(BitBoard &board)
    {
        assert(board);
        ESquare bit = lsb(board);
        board &= board - 1;
        return bit;
    }

    constexpr BitBoard operator&(BitBoard b, ESquare s) {return b & SquareBB(s);}
    constexpr BitBoard operator|(BitBoard b, ESquare s) {return b | SquareBB(s);}

    constexpr BitBoard operator&(ESquare s, BitBoard b) {return b & s;}
    constexpr BitBoard operator|(ESquare s, BitBoard b) {return b | s;}

    constexpr BitBoard operator|(ESquare s1, ESquare s2) {return SquareBB(s1) | s2;}
}