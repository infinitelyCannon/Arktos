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

        /*
         * Make Move (mutate board state)
         * Unname Move (mutate back)
         * Generate Legal Moves
         * Need a Move type
         * - from
         * - to
         * - is it a castle
         * - was there a capture
         * - was there a promotion
         * If we have the board history, why don't we rely on that to unmake moves?
         *
         */

    private:
        void SplitStr(std::vector<std::string>& list, const std::string& str, const char* subStr = " ") const;
        void SetStateInfo();
        void UpdateSliderBlockers(EColor color);

        BitBoard Pieces[PIECE_Invalid] = {0};
        BitBoard Colors[COLOR_None] = {0};
        EColor sideToMove = COLOR_White;
        int fullMoveClock = 0;
        std::vector<BoardState> StateHistory;

        /*TODO: Magic Bitboards
         * Data:
         * Add the rook, bishop, queen empty attack bitboards to the PseudoAtkBB (might not need this)
         * Add arrays for the sliding masks for MagicBB generations
         * 2D bitboard table for the actual rook/bishop hashmap
         * 2D array for ray attacks per direction for each square
         * Steps:
         * Precalculate the ray attacks
         * Precalculate sliding masks and magic numbers
         * For each square:
         * For each blocker combination 1 << indexBits:
         * get the blocker bitboard
         * do the operation to get the hash index
         * set the magicBB at that square/index to the calculated sliding attack
         */
        

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
