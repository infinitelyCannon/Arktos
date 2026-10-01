#pragma once

#include <cmath>
#include <cstdint>

namespace Arktos
{
    typedef uint64_t BitBoard;

    enum ESquare : unsigned int
    {
        SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
        SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
        SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
        SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
        SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
        SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
        SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
        SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
        SQ_ER
    };

    constexpr bool is_ok(ESquare const sq)
    {
        return sq >= SQ_A1 && sq <= SQ_H8;
    }

    enum EPiece : int
    {
        PIECE_Empty,
        PIECE_W_Pawn,
        PIECE_W_Knight,
        PIECE_W_Bishop,
        PIECE_W_Rook,
        PIECE_W_Queen,
        PIECE_W_King,
        PIECE_B_Pawn,
        PIECE_B_Knight,
        PIECE_B_Bishop,
        PIECE_B_Rook,
        PIECE_B_Queen,
        PIECE_B_King,
        PIECE_Invalid
    };

    enum  EAttackType : unsigned short int
    {
        ATK_King,
        ATK_Knight,
        ATK_W_Pawn,
        ATK_B_Pawn,
        ATK_Queen,
        ATK_Bishop,
        ATK_Rook,
        ATK_Invalid
    };

    enum EColor_OLD : unsigned short int
    {
	    White,
	    Black,
	    None
    };

    enum EColor : unsigned short int
    {
        COLOR_White,
        COLOR_Black,
        COLOR_None
    };

    struct BoardState
    {
	    //U64 Hash; Might have to be a string, gotta lookup how to generate hashes.
	    unsigned short CastlingRights = 0;
	    ESquare EnpassantSquare = SQ_ER;
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
            move |= static_cast<BitBoard>(capturedType) << 24;
            move |= static_cast<BitBoard>(movedType) << 19;
            move |= static_cast<BitBoard>(promotedType) << 14;
            move |= queenCastle ? 8192 : 0;
            move |= kingCastle ? 4096 : 0;
            move |= static_cast<uint32_t>(from) << 6;
            move |= static_cast<uint32_t>(to);
        }

        inline BitBoard GetTo() const {return 1ULL << (move & 0x3F);}
        inline BitBoard GetFrom() const {return 1ULL << (move >> 6 & 0x3F);}
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
        uint32_t move = 0;
    };
}