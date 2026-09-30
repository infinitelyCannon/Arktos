#include "Board.h"
#include "Exception.h"
#include <iostream>
#include <sstream>
#include <cassert>

Arktos::Board::Board()
{}

void Arktos::Board::Init()
{
    for (int c = COLOR_White; c < COLOR_None; ++c)
    {
        for (int sq = SQ_A1; sq < SQ_ER; ++sq)
        {
            BitBoard square = 1ULL << sq;
            int index = c == COLOR_White ? ATK_W_Pawn : ATK_B_Pawn;
            BitBoard east = c == COLOR_White ? square << 9 & ~FILE_A : square >> 7 & ~FILE_A;
            BitBoard west = c == COLOR_White ? square << 7 & ~FILE_H : square >> 9 & ~FILE_H;
            AttackBB[index][sq] = east | west;
        }
    }

    for (int sq = SQ_A1; sq < SQ_ER; ++sq)
    {
        BitBoard square = 1ULL << sq;
        BitBoard attacks = square << 1 & ~FILE_A | square >> 1 & ~FILE_H;
        attacks |= square;
        attacks |= attacks << 8 | attacks >> 8;
        AttackBB[ATK_King][sq] = attacks ^ square;

        BitBoard east, west;
        east = square << 1 & ~FILE_A;
        west = square >> 1 & ~FILE_H;
        attacks = (east | west) << 16;
        attacks |= (east | west) >> 16;
        east = east << 1 & ~FILE_A;
        west = west >> 1 & ~FILE_H;
        attacks |= (east | west) << 8;
        attacks |= (east | west) >> 8;
        AttackBB[ATK_Knight][sq] = attacks;
    }
}

void Arktos::Board::SetFEN(const std::string &fenStr)
{
    std::istringstream fenStream(fenStr == "startpos" ? START_POS : fenStr);
    char token;

    fenStream >> std::noskipws;
    StateHistory.clear();

    BitBoard sq = SquareBB(SQ_A8);

#define SET_BIT_SQR(type) \
    for (int p = PIECE_Empty; p != PIECE_Invalid; ++p) \
    { \
        Pieces[p] |= p == EPiece::type ? sq : 0; \
    }
#define SET_BLACK_BIT() \
    Colors[COLOR_White] |= 0; \
    Colors[COLOR_Black] |= sq;
#define SET_WHITE_BIT() \
    Colors[COLOR_White] |= sq; \
    Colors[COLOR_Black] |= 0;

    while ((fenStream >> token) && !isspace(token))
    {
        switch (token)
        {
            case 'r':
                SET_BIT_SQR(PIECE_B_Rook)
                SET_BLACK_BIT()
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'n':
                SET_BIT_SQR(PIECE_B_Knight)
                SET_BLACK_BIT()
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'b':
                SET_BIT_SQR(PIECE_B_Bishop)
                SET_BLACK_BIT()
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'q':
                SET_BIT_SQR(PIECE_B_Queen)
                SET_BLACK_BIT()
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'k':
                SET_BIT_SQR(PIECE_B_King)
                SET_BLACK_BIT()
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'p':
                SET_BIT_SQR(PIECE_B_Pawn)
                SET_BLACK_BIT()
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'R':
                SET_BIT_SQR(PIECE_W_Rook)
                SET_WHITE_BIT()
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'N':
                SET_BIT_SQR(PIECE_W_Knight)
                SET_WHITE_BIT()
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'B':
                SET_BIT_SQR(PIECE_W_Bishop)
                SET_WHITE_BIT()
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'Q':
                SET_BIT_SQR(PIECE_W_Queen)
                SET_WHITE_BIT()
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'K':
                SET_BIT_SQR(PIECE_W_King)
                SET_WHITE_BIT()
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'P':
                SET_BIT_SQR(PIECE_W_Pawn)
                SET_WHITE_BIT()
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case '/':
                sq = sq >> 8;
                break;
            case '8':
            case '7':
            case '6':
            case '5':
            case '4':
            case '3':
            case '2':
            case '1':
            {
                const int j = std::stoi(&token);
                const BitBoard end = sq << (j - 1);
                for (; sq <= end; sq = sq << 1)
                {
                    SET_BIT_SQR(PIECE_Empty)
                    Colors[COLOR_White] =
                        ~sq & Colors[COLOR_White];
                    Colors[COLOR_Black] =
                        ~sq & Colors[COLOR_Black];

                    if (sq == SquareBB(SQ_H8))
                    {
                        break;
                    }
                }
                sq = end & FILE_H ? end >> 7 : sq;
            }
                break;
            default:
                return;
        }
    }
#undef SET_BIT_SQR
#undef SET_BLACK_BIT
#undef SET_WHITE_BIT

    fenStream >> token;
    sideToMove = token == 'w' ? COLOR_White : COLOR_Black;
    fenStream >> token;

    StateHistory.emplace_back();

    while ((fenStream >> token) && !isspace(token))
    {
        switch (token)
        {
            case 'K':
                StateHistory[0].CastlingRights |= K_CAST;
                break;
            case 'Q':
                StateHistory[0].CastlingRights |= Q_CAST;
                break;
            case 'k':
                StateHistory[0].CastlingRights |= k_CAST;
                break;
            case 'q':
                StateHistory[0].CastlingRights |= q_CAST;
                break;
            case '-':
            default:
                StateHistory[0].CastlingRights = 0;
                break;
        }
    }

    fenStream >> token;
    if (token != '-')
    {
        char file = token;
        switch (file)
        {
            case 'a':
                file = 0;
                break;
            case 'b':
                file = 1;
                break;
            case 'c':
                file = 2;
                break;
            case 'd':
                file = 3;
                break;
            case 'e':
                file = 4;
                break;
            case 'f':
                file = 5;
                break;
            case 'g':
                file = 6;
                break;
            case 'h':
                file = 7;
                break;
            default:
                file = 0;
                break;
        }

        fenStream >> token;
        const int rank = std::stoi(&token);

        StateHistory[0].EnpassantSquare = static_cast<ESquare>((rank - 1) * 8 + file);
    }

    fenStream >> std::skipws;
    fenStream >> token;
    StateHistory[0].Repetitions = std::stoi(&token);

    fenStream >> token;
    fullMoveClock = std::stoi(&token);
}

void Arktos::Board::CheckFEN(std::string fen)
{
    BitBoard sq = SquareBB(SQ_A8);
    std::istringstream fenStream(fen);
    char token;

    fenStream >> std::noskipws;

    while ((fenStream >> token) && !isspace(token))
    {
        switch (token)
        {
            case 'r':
                StateException::Check(Pieces[PIECE_B_Rook] & sq, "Black Rook bitboard does not match FEN string.");
                StateException::Check(!(Pieces[PIECE_Empty] & sq), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(Colors[COLOR_Black] & sq, "Black bitboard does not match FEN string.");
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'n':
                StateException::Check(Pieces[PIECE_B_Knight] & sq, "Black Knight bitboard does not match FEN string.");
                StateException::Check(!(Pieces[PIECE_Empty] & sq), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(Colors[COLOR_Black] & sq, "Black bitboard does not match FEN string.");
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'b':
                StateException::Check(Pieces[PIECE_B_Bishop] & sq, "Black Bishop bitboard does not match FEN string.");
                StateException::Check(!(Pieces[PIECE_Empty] & sq), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(Colors[COLOR_Black] & sq, "Black bitboard does not match FEN string.");
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'q':
                StateException::Check(Pieces[PIECE_B_Queen] & sq, "Black Queen bitboard does not match FEN string.");
                StateException::Check(!(Pieces[PIECE_Empty] & sq), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(Colors[COLOR_Black] & sq, "Black bitboard does not match FEN string.");
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'k':
                StateException::Check(Pieces[PIECE_B_King] & sq, "Black King bitboard does not match FEN string.");
                StateException::Check(!(Pieces[PIECE_Empty] & sq), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(Colors[COLOR_Black] & sq, "Black bitboard does not match FEN string.");
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'p':
                StateException::Check(Pieces[PIECE_B_Pawn] & sq, "Black Pawn bitboard does not match FEN string.");
                StateException::Check(!(Pieces[PIECE_Empty] & sq), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(Colors[COLOR_Black] & sq, "Black bitboard does not match FEN string.");
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'R':
                StateException::Check(Pieces[PIECE_W_Rook] & sq, "White Rook bitboard does not match FEN string.");
                StateException::Check(!(Pieces[PIECE_Empty] & sq), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(Colors[COLOR_White] & sq, "White bitboard does not match FEN string.");
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'N':
                StateException::Check(Pieces[PIECE_W_Knight] & sq, "White Knight bitboard does not match FEN string.");
                StateException::Check(!(Pieces[PIECE_Empty] & sq), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(Colors[COLOR_White] & sq, "White bitboard does not match FEN string.");
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'B':
                StateException::Check(Pieces[PIECE_W_Bishop] & sq, "White Bishop bitboard does not match FEN string.");
                StateException::Check(!(Pieces[PIECE_Empty] & sq), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(Colors[COLOR_White] & sq, "White bitboard does not match FEN string.");
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'Q':
                StateException::Check(Pieces[PIECE_W_Queen] & sq, "White Queen bitboard does not match FEN string.");
                StateException::Check(!(Pieces[PIECE_Empty] & sq), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(Colors[COLOR_White] & sq, "White bitboard does not match FEN string.");
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'K':
                StateException::Check(Pieces[PIECE_W_King] & sq, "White King bitboard does not match FEN string.");
                StateException::Check(!(Pieces[PIECE_Empty] & sq), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(Colors[COLOR_White] & sq, "White bitboard does not match FEN string.");
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case 'P':
                StateException::Check(Pieces[PIECE_W_Pawn] & sq, "White Pawn bitboard does not match FEN string.");
                StateException::Check(!(Pieces[PIECE_Empty] & sq), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(Colors[COLOR_White] & sq, "White bitboard does not match FEN string.");
                sq = sq & FILE_H ? sq >> 7 : sq << 1;
                break;
            case '/':
                sq = sq >> 8;
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            {
                const int j = std::stoi(&token);
                const BitBoard end = sq << (j - 1);

                for (; sq <= end; sq = sq << 1)
                {
                    StateException::Check(Pieces[PIECE_Empty] & sq, "Bitboard reported as not empty for empty FEN square.");
                    StateException::Check(!(Colors[COLOR_Black] & sq), "Black bitboard reported occupied for an empty FEN square.");
                    StateException::Check(!(Colors[COLOR_White] & sq), "White bitboard reported occupied for an empty FEN square.");
                    for (int p = PIECE_W_Pawn; p != PIECE_Invalid; ++p)
                    {
                        std::ostringstream msg;
                        msg << "Bitboard index (" << p << ") reported occupied for an empty FEN square";
                        StateException::Check(!(Pieces[p] & sq), msg.str().c_str());
                    }
                }

                sq = end & FILE_H ? end >> 7 : sq;
            }
                break;

            default:
            {
                std::string msg = std::string("Unrecognized token in FEN string. ") + std::string ({'(', token, ')'});
                throw std::invalid_argument(msg.c_str());
            }
        }
    }

    fenStream >> token;
    if (token == 'w')
    {
        StateException::Check(sideToMove == COLOR_White, "Wrong side to move reported (Expected White)");
    }
    else if (token == 'b')
    {
        StateException::Check(sideToMove == COLOR_Black, "Wrong side to move reported (Exptected Black)");
    }
    else
    {
        throw std::invalid_argument("Invalid token passed in for side to move");
    }

    fenStream >> token;

    while ((fenStream >> token) && !isspace(token))
    {
        switch (token)
        {
            case 'K':
                StateException::Check(StateHistory.back().CastlingRights & K_CAST, "White king is missing castling rights.");
                break;
            case 'Q':
                StateException::Check(StateHistory.back().CastlingRights & Q_CAST, "White queen is missing castling rights.");
                break;
            case 'k':
                StateException::Check(StateHistory.back().CastlingRights & k_CAST, "Black king is missing castling rights.");
                break;
            case 'q':
                StateException::Check(StateHistory.back().CastlingRights & q_CAST, "Black queen is missing castling rights.");
                break;
            case '-':
                StateException::Check(StateHistory.back().CastlingRights == 0, "Castling rights reported as not blank when it should be.");
                break;
            default:
                throw std::invalid_argument("Unknown token found for Castling rights");
                break;
        }
    }

    fenStream >> token;
    ESquare enPassant = SQ_ER;
    if (token != '-')
    {
        char file = token;
        switch (file)
        {
            case 'a':
                file = 0;
                break;
            case 'b':
                file = 1;
                break;
            case 'c':
                file = 2;
                break;
            case 'd':
                file = 3;
                break;
            case 'e':
                file = 4;
                break;
            case 'f':
                file = 5;
                break;
            case 'g':
                file = 6;
                break;
            case 'h':
                file = 7;
                break;
            default:
                file = 0;
                break;
        }

        fenStream >> token;
        const int rank = std::stoi(&token);

        enPassant = static_cast<ESquare>((rank - 1) * 8 + file);
    }

    StateException::Check(StateHistory.back().EnpassantSquare == enPassant, "EnPassant Square mismatch.");

    fenStream >> std::skipws;
    fenStream >> token;
    StateException::Check(StateHistory.back().Repetitions == std::stoi(&token), "Half-clock values do not match.");

    fenStream >> token;
    StateException::Check(fullMoveClock == std::stoi(&token), "Full move clock values do not match.");
}

Arktos::Move Arktos::Board::ParseMove(std::string str) const
{
    ESquare from = SQ_ER;
    ESquare to = SQ_ER;
    EPiece movedType = PIECE_Invalid;
    EPiece capturedType = PIECE_Invalid;
    EPiece promotedType = PIECE_Invalid;
    bool kingCastle = false;
    bool queenCastle = false;
    int file;
    int rank;

    switch (str[0])
    {
        case 'a':
            file = 0;
            break;
        case 'b':
            file = 1;
            break;
        case 'c':
            file = 2;
            break;
        case 'd':
            file = 3;
            break;
        case 'e':
            file = 4;
            break;
        case 'f':
            file = 5;
            break;
        case 'g':
            file = 6;
            break;
        case 'h':
            file = 7;
            break;
        default:
            return Move();
            break;
    }
    rank = std::stoi(&str[1]);
    from = static_cast<ESquare>((rank - 1) * 8 + file);

    switch (str[2])
    {
        case 'a':
            file = 0;
            break;
        case 'b':
            file = 1;
            break;
        case 'c':
            file = 2;
            break;
        case 'd':
            file = 3;
            break;
        case 'e':
            file = 4;
            break;
        case 'f':
            file = 5;
            break;
        case 'g':
            file = 6;
            break;
        case 'h':
            file = 7;
            break;
        default:
            return Move();
            break;
    }
    rank = std::stoi(&str[3]);
    to = static_cast<ESquare>((rank - 1) * 8 + file);

    const int pieceIdx = sideToMove == COLOR_White ? PIECE_W_Pawn :
        PIECE_B_Pawn;

    if (Pieces[pieceIdx] & SquareBB(from) && to == StateHistory.back().EnpassantSquare)
    {
        capturedType = sideToMove == COLOR_White ? PIECE_B_Pawn : PIECE_W_Pawn;
    }
    else if (Pieces[PIECE_Empty] & SquareBB(to))
    {
        for (int p = PIECE_W_Pawn; p != PIECE_Invalid; ++p)
        {
            if (Pieces[p] & SquareBB(to))
            {
                capturedType = static_cast<EPiece>(p);
                break;
            }
        }
    }

    for (int p = PIECE_W_Pawn; p != PIECE_Invalid; ++p)
    {
        if (Pieces[p] & SquareBB(from))
        {
            movedType = static_cast<EPiece>(p);
            break;
        }
    }

    switch (str.back())
    {
        case 'q':
            promotedType = sideToMove == COLOR_White ? PIECE_W_Queen : PIECE_B_Queen;
            break;
        case 'r':
            promotedType = sideToMove == COLOR_White ? PIECE_W_Rook : PIECE_B_Rook;
            break;
        case 'b':
            promotedType = sideToMove == COLOR_White ? PIECE_W_Bishop : PIECE_B_Bishop;
            break;
        case 'k':
            promotedType = sideToMove == COLOR_White ? PIECE_W_Knight : PIECE_B_Knight;
            break;
    }

    if (sideToMove == COLOR_White)
    {
        kingCastle = movedType == PIECE_W_King
        && from == SQ_E1
        && to == SQ_G1;

        queenCastle = movedType == PIECE_W_King
        && from == SQ_E1
        && to == SQ_C1;
    }
    else if (sideToMove == COLOR_Black)
    {
        kingCastle = movedType == PIECE_B_King
        && from == SQ_E8
        && to == SQ_G8;

        queenCastle = movedType == PIECE_B_King
        && from == SQ_E8
        && to == SQ_C8;
    }

    return Move(
        capturedType,
        movedType,
        promotedType,
        queenCastle,
        kingCastle,
        from,
        to);
}

std::vector<Arktos::Move> Arktos::Board::GenerateLegalMoves() const
{
    std::vector<Arktos::Move> moves;

    /*
     * Generate a BitBoard of moves a piece is allowed to make (i.e. spaces that are empty or have an enemy piece).
     * For all the bits in that board, build a move for that from/to, test for check (make, test, unmake), and add to list if legal
     */

    return moves;
}

/*
 *TODO:
 * bool CanEnPassant()
 * check if there is an enemy pawn directly adjacent to the moving pawn
 * might just have to do a 'is _ king in check in _ board position'
 * then have that run for the pawn(s) that can attack.
 */

void Arktos::Board::MakeMove(Move move)
{
    const BitBoard dest = move.GetTo();
    const BitBoard start = move.GetFrom();
    const EPiece movedType = move.GetMovedPiece();
    const EPiece capturedType = move.GetCapturedPiece();
    const EPiece promotedType = move.GetPromotedPiece();
    BoardState newState(StateHistory.back());
    bool checkEP = false;
    ESquare toSq = lsb(dest);

    /*
     * See if the other side is in check
     */

    assert(sideToMove != COLOR_None);
    Pieces[movedType] ^= start | dest;
    Pieces[PIECE_Empty] ^= start | dest;
    Colors[sideToMove] ^= start | dest;
    newState.EnpassantSquare = SQ_ER;

    if (sideToMove == COLOR_White &&
        movedType == PIECE_W_Pawn &&
        capturedType == PIECE_B_Pawn &&
        lsb(dest) == StateHistory.back().EnpassantSquare)
    {
        Pieces[PIECE_B_Pawn] ^= dest << 8;
        Colors[COLOR_Black] ^= dest << 8;
        newState.EnpassantSquare = SQ_ER;
    }
    else if (sideToMove == COLOR_Black &&
            movedType == PIECE_B_Pawn &&
            capturedType == PIECE_W_Pawn &&
            lsb(dest) == StateHistory.back().EnpassantSquare)
    {
        Pieces[PIECE_W_Pawn] ^= dest >> 8;
        Colors[COLOR_White] ^= dest >> 8;
        newState.EnpassantSquare = SQ_ER;
    }
    else if (capturedType != PIECE_Invalid)
    {
        Pieces[capturedType] ^= dest;
        Colors[sideToMove == COLOR_White ? COLOR_Black : COLOR_White] ^= dest;
    }

    if (move.KingSideCastle())
    {
        if (sideToMove == COLOR_White)
        {
            Pieces[PIECE_W_Rook] ^= SquareBB(SQ_H1) | SquareBB(SQ_F1);
            Colors[COLOR_White] ^= SquareBB(SQ_H1) | SquareBB(SQ_F1);
            newState.CastlingRights &= k_CAST | q_CAST;
        }
        else
        {
            Pieces[PIECE_B_Rook] ^= SquareBB(SQ_H8) | SquareBB(SQ_F8);
            Colors[COLOR_Black] ^= SquareBB(SQ_H8) | SquareBB(SQ_F8);
            newState.CastlingRights &= K_CAST | Q_CAST;
        }
    }
    else if (move.QueenSideCastle())
    {
        if (sideToMove == COLOR_White)
        {
            Pieces[PIECE_W_Rook] ^= SquareBB(SQ_A1) | SquareBB(SQ_D1);
            Colors[COLOR_White] ^= SquareBB(SQ_A1) | SquareBB(SQ_D1);
            newState.CastlingRights &= k_CAST | q_CAST;
        }
        else
        {
            Pieces[PIECE_B_Rook] ^= SquareBB(SQ_A8) | SquareBB(SQ_D8);
            Colors[COLOR_Black] ^= SquareBB(SQ_A8) | SquareBB(SQ_D8);
            newState.CastlingRights &= K_CAST | Q_CAST;
        }
    }

    if (movedType == PIECE_W_Pawn || movedType == PIECE_B_Pawn)
    {
        if (promotedType != PIECE_Invalid)
        {
            Pieces[static_cast<int>(movedType)] ^= dest;
            Pieces[static_cast<int>(promotedType)] ^= dest;
            newState.EnpassantSquare = SQ_ER;
        }
        /*
         *TODO:
         * Other chess engines ONLY update Enpassant when the side to move legally can perform one
         * So instead of what I'm doing now where it updates after every double push,
         * I need to check if a pawn can take this turn, and check for legality
         * (see if that move puts the King in check)
         * So then I'll need a way to take a given bitboard and see if a given square is in check
         */
        /* checkEP:
        * Pawn double pushed
        - at least one adjacent enemy pawn
    
        EP is legal the EP capturing pawn is not pinned
         */
        const EColor oppSide = sideToMove == COLOR_White ? COLOR_Black : COLOR_White;
        const ESquare attackSq = sideToMove == COLOR_White ? lsb(dest >> 8) : lsb(dest << 8);

        checkEP = (lsb(dest) ^ lsb(start)) == 16 &&
        AttackBB[ATK_W_Pawn + sideToMove][attackSq] & Pieces[oppSide == COLOR_Black ? PIECE_B_Pawn : PIECE_W_Pawn];

        std::cout << "CheckEP: " << checkEP << std::endl;
    }

    while (false)
    {
        ESquare to = lsb(dest);

    }

    if (capturedType != PIECE_Invalid || movedType == PIECE_W_Pawn || movedType == PIECE_B_Pawn)
    {
        newState.Repetitions = 0;
    }
    else
    {
        newState.Repetitions++;
    }

    if (sideToMove == COLOR_Black)
    {
        fullMoveClock++;
    }

    sideToMove = sideToMove == COLOR_White ? COLOR_Black : COLOR_White;

    StateHistory.push_back(newState);
}

void Arktos::Board::UnMakeMove(Move move)
{

}

std::string Arktos::Board::GetBitBoardStr(EPiece type) const
{
    constexpr BitBoard bit = 1LL << 63;
    const BitBoard board = Pieces[type];
    std::string str = "\n +---+---+---+---+---+---+---+---+  \n";

    for (int i = 0; i < 8; ++i)
    {
        str += " |";
        for (int j = 7; j >= 0; --j)
        {
            str += std::string(" ") + std::string(board & bit >> (i * 8 + j) ? "1 |" : "0 |");
        }
        str += " " + std::to_string(8 - i) + "\n";
    }

    str += " +---+---+---+---+---+---+---+---+\n   a   b   c   d   e   f   g   h    \n";
    return str;
}

void Arktos::Board::SplitStr(std::vector<std::string>& list, const std::string& str, const char* subStr) const
{
	std::string::size_type index = str.find(subStr);
	std::string::size_type start = 0;

	while (index != std::string::npos)
	{
	    if (start != index)
	    {
	        list.push_back(str.substr(start, index - start));
	    }
		start = ++index;
		index = str.find(subStr, start);
	}

    if (start != str.length())
    {
        list.push_back(str.substr(start));
    }
}

Arktos::Board::~Board()
{}
