#pragma once

#include <cmath>

namespace Arktos
{
    typedef unsigned long long U64;

    #define BIT(s, b) s = 1ULL << b
    enum class ESquare : U64
    {
        BIT(A1, 0), BIT(B1, 1), BIT(C1, 2), BIT(D1, 3), BIT(E1, 4), BIT(F1, 5), BIT(G1, 6), BIT(H1, 7),
	    BIT(A2, 8), BIT(B2, 9), BIT(C2, 10), BIT(D2, 11), BIT(E2, 12), BIT(F2, 13), BIT(G2, 14), BIT(H2, 15),
	    BIT(A3, 16), BIT(B3, 17), BIT(C3, 18), BIT(D3, 19), BIT(E3, 20), BIT(F3, 21), BIT(G3, 22), BIT(H3, 23),
	    BIT(A4, 24), BIT(B4, 25), BIT(C4, 26), BIT(D4, 27), BIT(E4, 28), BIT(F4, 29), BIT(G4, 30), BIT(H4, 31),
	    BIT(A5, 32), BIT(B5, 33), BIT(C5, 34), BIT(D5, 35), BIT(E5, 36), BIT(F5, 37), BIT(G5, 38), BIT(H5, 39),
	    BIT(A6, 40), BIT(B6, 41), BIT(C6, 42), BIT(D6, 43), BIT(E6, 44), BIT(F6, 45), BIT(G6, 46), BIT(H6, 47),
	    BIT(A7, 48), BIT(B7, 49), BIT(C7, 50), BIT(D7, 51), BIT(E7, 52), BIT(F7, 53), BIT(G7, 54), BIT(H7, 55),
	    BIT(A8, 56), BIT(B8, 57), BIT(C8, 58), BIT(D8, 59), BIT(E8, 60), BIT(F8, 61), BIT(G8, 62), BIT(H8, 63),
        ER = 0
    };
    #undef BIT

    inline ESquare operator+  (const ESquare &square, const int &b) {return square == ESquare::H8 ? ESquare::H8 : static_cast<ESquare>(static_cast<U64>(square) << b);}
    inline ESquare operator-  (const ESquare &square, const int &b) {return square == ESquare::A1 ? ESquare::A1 : static_cast<ESquare>(static_cast<U64>(square) >> b);}
    inline ESquare operator++ (ESquare &square) {square = square + 1; return square;}
    inline ESquare operator++ (ESquare &square, int) {ESquare old = square; square = square + 1; return old;}

    enum class EPiece : unsigned short int
    {
	    Empty,
	    W_Pawn,
	    W_Knight,
	    W_Bishop,
	    W_Rook,
	    W_Queen,
	    W_King,
	    B_Pawn,
	    B_Knight,
	    B_Bishop,
	    B_Rook,
	    B_Queen,
	    B_King,
	    Invalid
    };

    enum class EAttackType : unsigned short int
    {
        King,
        Queen,
        Bishop,
        Knight,
        Rook,
        W_Pawn,
        B_Pawn
    };

    inline EPiece operator++ (EPiece& piece) { piece = static_cast<EPiece>(static_cast<int>(piece) + 1); return piece; }
    inline EPiece operator++ (EPiece& piece, int) { EPiece old = piece; piece = static_cast<EPiece>(static_cast<int>(piece) + 1); return old; }

    enum class EColor : unsigned short int
    {
	    White,
	    Black,
	    None
    };

    struct BitBoard
    {
	    U64 Pieces[static_cast<int>(EPiece::B_King) + 1] = {};
	    U64 PiecesByColor[static_cast<short>(EColor::Black) + 1] = {};
        U64 AttackBB[static_cast<int>(EAttackType::B_Pawn) + 1] = {};
    };

    struct BoardState
    {
	    //U64 Hash; Might have to be a string, gotta lookup how to generate hashes.
	    unsigned short CastlingRights = 0;
	    ESquare EnpassantSquare = ESquare::ER;
	    bool InCheck = false;
	    int Repetitions = 0;
    };

    struct Move
    {
        Move() = default;
        Move(EPiece capturedType, EPiece movedType, EPiece promotedType,
            bool queenCastle,
            bool kingCastle,
            ESquare from,
            ESquare to) : move(0)
        {
            move |= static_cast<U64>(capturedType) << 24;
            move |= static_cast<U64>(movedType) << 19;
            move |= static_cast<U64>(promotedType) << 14;
            move |= queenCastle ? 8192 : 0;
            move |= kingCastle ? 4096 : 0;
            move |= static_cast<unsigned int>(std::log2(static_cast<U64>(from))) << 6;
            move |= static_cast<unsigned int>(std::log2(static_cast<U64>(to)));
        }

        inline U64 GetTo() const {return 1ULL << (move & 0x3F);}
        inline U64 GetFrom() const {return 1ULL << (move >> 6 & 0x3F);}
        inline EPiece GetMovedPiece() const {return static_cast<EPiece>(move >> 19 & 0x1F);}
        inline EPiece GetCapturedPiece() const {return static_cast<EPiece>(move >> 24 & 0x1F);}
        inline bool KingSideCastle() const {return move & 4096;}
        inline bool QueenSideCastle() const {return move & 8192;}
        inline EPiece GetPromotedPiece() const {return static_cast<EPiece>(move >> 14 & 0x1F);}

    private:
        /*
         * From MSB to LSB:
         * Unused (3)
         * Captured EPiece Type (int index mapped to BitBoard array, if valid) (5)
         * Moving EPiece Type (int index mapped to BitBoard array) (5)
         * Promoted EPiece Type (int index mapped to BitBoard array, if valid) (5)
         * Queen Side Castle (1)
         * King Side Castle (1)
         * From ESquare (a 0-63 int for left shifting) (6)
         * To ESquare (a 0-63 int for left shifting) (6)
        */
        unsigned int move = 0;
    };
}