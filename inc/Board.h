#pragma once

#include <vector>
#include <string>
#include "Types.h"

namespace Arktos
{
    class Board
    {
    public:
        Board();
        ~Board();

        void Init(const std::string &fenStr);
        std::string GetBitBoardStr(EPiece type) const;

        void MakeMove(Move move);
        void UnMakeMove(Move move);
        std::vector<Move> GenerateLegalMoves() const;
        void CheckFEN(std::string fen);
        Move ParseMove(std::string str) const;

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

        BitBoard bitBoard;
        EColor sideToMove = EColor::White;
        int fullMoveClock = 0;
        std::vector<BoardState> StateHistory;
        static constexpr U64 FILE_H = 0x8080808080808080;
        static constexpr U64 RANK_1 = 0xFF;
        static constexpr U64 RANK_2 = 0xFF00;
        static constexpr U64 RANK_4 = 0xFF000000;
        static constexpr U64 RANK_5 = 0xFF00000000;
        static constexpr U64 RANK_7 = 0xFF000000000000;
        static constexpr U64 RANK_8 = 0xFF00000000000000;

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
