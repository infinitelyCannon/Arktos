#pragma once

#include <vector>
#include <string>
#include "BitBoard.h"

namespace Arktos
{
    class Board
    {
    public:
        Board();
        ~Board();

        void Init();
        void SetFEN(const std::string &fenStr);
        std::string GetBitBoardStr(EPiece type) const;

        void MakeMove(Move move);
        void UnMakeMove(Move move);
        std::vector<Move> GenerateLegalMoves() const;
        void CheckFEN(std::string fen);
        Move ParseMove(std::string str) const;

        
        inline BitBoard GetPieces(PieceType pt) const
        {
            if (pt == PIECE_TYPE_NONE)
                return Pieces[PIECE_Empty];
            if (pt == ALL_PIECES)
                return ~Pieces[PIECE_Empty];

            return Pieces[pt] | Pieces[pt + 6];
        }

        inline BitBoard GetPieces(EColor c) const
        {
            assert(c != COLOR_None);
            return Colors[c];
        }

        inline BitBoard GetPieces(EColor c, PieceType pt) const
        {
            assert(pt != ALL_PIECES && pt != PIECE_TYPE_NONE);
            return c == COLOR_White ? Pieces[pt] : Pieces[pt + 6];
        }

        inline void RemoveCastlingForSide(EColor side, BoardState& state)
        {
            assert(side != COLOR_None);
            state.CastlingRights &= k_CAST << (side * 2) | q_CAST << (side * 2);
        }

        template<PieceType pt>
        inline void MovePiece(EColor c, BitBoard from, BitBoard to)
        {
            assert(c != COLOR_None && pt != PIECE_TYPE_NONE && pt != ALL_PIECES);
            Pieces[pt + (c * 6)] ^= from | to;
            Pieces[PIECE_Empty] ^= from | to;
            Colors[c] ^= from | to;
        }

        template<PieceType pt>
        inline void AddPiece(EColor c, BitBoard sq)
        {
            assert(c != COLOR_None && pt != ALL_PIECES && pt != PIECE_TYPE_NONE);
            Pieces[pt + (c * 6)] |= sq;
            Colors[c] |= sq;
            Pieces[PIECE_Empty] &= ~sq;
        }

        template<PieceType pt>
        inline void RemovePiece(EColor c, BitBoard sq)
        {
            assert(pt != ALL_PIECES && pt != PIECE_TYPE_NONE && c != COLOR_None);
            Pieces[PIECE_Empty] |= sq;
            Pieces[pt + (c * 6)] &= ~sq;
            Colors[c] &= ~sq;
        }

        inline void RemovePiece(EPiece p, EColor c, BitBoard sq)
        {
            assert(p != PIECE_Invalid && p != PIECE_Empty);
            Pieces[PIECE_Empty] |= sq;
            Pieces[p] &= ~sq;
            Colors[c] &= ~sq;
        }

        inline void MovePiece(EPiece p, EColor c, BitBoard from, BitBoard to)
        {
            assert(p != PIECE_Invalid && p != PIECE_Empty);
            Pieces[p] ^= from | to;
            Pieces[PIECE_Empty] ^= from | to;
            Colors[c] ^= from | to;
        }

        inline void AddPiece(EPiece p, EColor c, BitBoard sq)
        {
            assert(p != PIECE_Invalid && p != PIECE_Empty);
            Pieces[p] |= sq;
            Pieces[PIECE_Empty] &= ~sq;
            Colors[c] |= sq;
        }

    private:
        void SplitStr(std::vector<std::string>& list, const std::string& str, const char* subStr = " ") const;
        void SetStateInfo();
        void UpdateSliderBlockers(EColor color);
        void Reset()
        {
            StateHistory.clear();
            sideToMove = COLOR_White;
            fullMoveClock = 0;
            for (int i = PIECE_Empty; i != PIECE_Invalid; ++i)
            {
                Pieces[i] = 0;
            }
            Colors[COLOR_White] = 0;
            Colors[COLOR_Black] = 0;
        }

        BitBoard Pieces[PIECE_Invalid] = {0};
        BitBoard Colors[COLOR_None] = {0};
        EColor sideToMove = COLOR_White;
        int fullMoveClock = 0;
        std::vector<BoardState> StateHistory;

        /*
          ____ ____ ____ KQkq
          0000 0000 0000 0000
         */
        static constexpr unsigned short K_CAST = 8;
        static constexpr unsigned short Q_CAST = 4;
        static constexpr unsigned short k_CAST = 2;
        static constexpr unsigned short q_CAST = 1;
        static constexpr char const * START_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    };
}
