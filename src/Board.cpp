#include "Board.h"
#include "Exception.h"
#include <iostream>
#include <sstream>
#include <cassert>

Arktos::Board::Board()
{}

void Arktos::Board::Init(const std::string &fenStr)
{
    std::istringstream fenStream(fenStr == "startpos" ? START_POS : fenStr);
    char token;

    fenStream >> std::noskipws;
    StateHistory.clear();

    ESquare sq = ESquare::A8;

#define SET_BIT_SQR(type) \
    for (EPiece p = EPiece::Empty; p != EPiece::Invalid; ++p) \
    { \
        bitBoard.Pieces[static_cast<int>(p)] |= p == EPiece::type ? static_cast<U64>(sq) : 0; \
    }
#define SET_BLACK_BIT() \
    bitBoard.PiecesByColor[static_cast<int>(EColor::White)] |= 0; \
    bitBoard.PiecesByColor[static_cast<int>(EColor::Black)] |= static_cast<U64>(sq);
#define SET_WHITE_BIT() \
    bitBoard.PiecesByColor[static_cast<int>(EColor::White)] |= static_cast<U64>(sq); \
    bitBoard.PiecesByColor[static_cast<int>(EColor::Black)] |= 0;

    while ((fenStream >> token) && !isspace(token))
    {
        switch (token)
        {
            case 'r':
                SET_BIT_SQR(B_Rook)
                SET_BLACK_BIT()
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'n':
                SET_BIT_SQR(B_Knight)
                SET_BLACK_BIT()
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'b':
                SET_BIT_SQR(B_Bishop)
                SET_BLACK_BIT()
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'q':
                SET_BIT_SQR(B_Queen)
                SET_BLACK_BIT()
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'k':
                SET_BIT_SQR(B_King)
                SET_BLACK_BIT()
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'p':
                SET_BIT_SQR(B_Pawn)
                SET_BLACK_BIT()
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'R':
                SET_BIT_SQR(W_Rook)
                SET_WHITE_BIT()
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'N':
                SET_BIT_SQR(W_Knight)
                SET_WHITE_BIT()
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'B':
                SET_BIT_SQR(W_Bishop)
                SET_WHITE_BIT()
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'Q':
                SET_BIT_SQR(W_Queen)
                SET_WHITE_BIT()
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'K':
                SET_BIT_SQR(W_King)
                SET_WHITE_BIT()
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'P':
                SET_BIT_SQR(W_Pawn)
                SET_WHITE_BIT()
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case '/':
                sq = sq - 8;
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
                const ESquare end = sq + (j - 1);
                for (; sq <= end; ++sq)
                {
                    SET_BIT_SQR(Empty)
                    bitBoard.PiecesByColor[static_cast<int>(EColor::White)] =
                        ~static_cast<U64>(sq) & bitBoard.PiecesByColor[static_cast<int>(EColor::White)];
                    bitBoard.PiecesByColor[static_cast<int>(EColor::Black)] =
                        ~static_cast<U64>(sq) & bitBoard.PiecesByColor[static_cast<int>(EColor::Black)];
                }
                sq = static_cast<U64>(end) & FILE_H ? end - 7 : sq;
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
    sideToMove = token == 'w' ? EColor::White : EColor::Black;
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

        StateHistory[0].EnpassantSquare = static_cast<ESquare>(1ULL << ((rank - 1) * 8 + file));
    }

    fenStream >> std::skipws;
    fenStream >> token;
    StateHistory[0].Repetitions = std::stoi(&token);

    fenStream >> token;
    fullMoveClock = std::stoi(&token);

    std::cout << "Ready." << std::endl;
}

void Arktos::Board::CheckFEN(std::string fen)
{
    ESquare sq = ESquare::A8;
    std::istringstream fenStream(fen);
    char token;

    fenStream >> std::noskipws;

    while ((fenStream >> token) && !isspace(token))
    {
        switch (token)
        {
            case 'r':
                StateException::Check(bitBoard.Pieces[static_cast<int>(EPiece::B_Rook)] & static_cast<U64>(sq), "Black Rook bitboard does not match FEN string.");
                StateException::Check(!(bitBoard.Pieces[static_cast<int>(EPiece::Empty)] & static_cast<U64>(sq)), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(bitBoard.PiecesByColor[static_cast<int>(EColor::Black)] & static_cast<U64>(sq), "Black bitboard does not match FEN string.");
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'n':
                StateException::Check(bitBoard.Pieces[static_cast<int>(EPiece::B_Knight)] & static_cast<U64>(sq), "Black Knight bitboard does not match FEN string.");
                StateException::Check(!(bitBoard.Pieces[static_cast<int>(EPiece::Empty)] & static_cast<U64>(sq)), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(bitBoard.PiecesByColor[static_cast<int>(EColor::Black)] & static_cast<U64>(sq), "Black bitboard does not match FEN string.");
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'b':
                StateException::Check(bitBoard.Pieces[static_cast<int>(EPiece::B_Bishop)] & static_cast<U64>(sq), "Black Bishop bitboard does not match FEN string.");
                StateException::Check(!(bitBoard.Pieces[static_cast<int>(EPiece::Empty)] & static_cast<U64>(sq)), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(bitBoard.PiecesByColor[static_cast<int>(EColor::Black)] & static_cast<U64>(sq), "Black bitboard does not match FEN string.");
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'q':
                StateException::Check(bitBoard.Pieces[static_cast<int>(EPiece::B_Queen)] & static_cast<U64>(sq), "Black Queen bitboard does not match FEN string.");
                StateException::Check(!(bitBoard.Pieces[static_cast<int>(EPiece::Empty)] & static_cast<U64>(sq)), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(bitBoard.PiecesByColor[static_cast<int>(EColor::Black)] & static_cast<U64>(sq), "Black bitboard does not match FEN string.");
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'k':
                StateException::Check(bitBoard.Pieces[static_cast<int>(EPiece::B_King)] & static_cast<U64>(sq), "Black King bitboard does not match FEN string.");
                StateException::Check(!(bitBoard.Pieces[static_cast<int>(EPiece::Empty)] & static_cast<U64>(sq)), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(bitBoard.PiecesByColor[static_cast<int>(EColor::Black)] & static_cast<U64>(sq), "Black bitboard does not match FEN string.");
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'p':
                StateException::Check(bitBoard.Pieces[static_cast<int>(EPiece::B_Pawn)] & static_cast<U64>(sq), "Black Pawn bitboard does not match FEN string.");
                StateException::Check(!(bitBoard.Pieces[static_cast<int>(EPiece::Empty)] & static_cast<U64>(sq)), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(bitBoard.PiecesByColor[static_cast<int>(EColor::Black)] & static_cast<U64>(sq), "Black bitboard does not match FEN string.");
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'R':
                StateException::Check(bitBoard.Pieces[static_cast<int>(EPiece::W_Rook)] & static_cast<U64>(sq), "White Rook bitboard does not match FEN string.");
                StateException::Check(!(bitBoard.Pieces[static_cast<int>(EPiece::Empty)] & static_cast<U64>(sq)), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(bitBoard.PiecesByColor[static_cast<int>(EColor::White)] & static_cast<U64>(sq), "White bitboard does not match FEN string.");
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'N':
                StateException::Check(bitBoard.Pieces[static_cast<int>(EPiece::W_Knight)] & static_cast<U64>(sq), "White Knight bitboard does not match FEN string.");
                StateException::Check(!(bitBoard.Pieces[static_cast<int>(EPiece::Empty)] & static_cast<U64>(sq)), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(bitBoard.PiecesByColor[static_cast<int>(EColor::White)] & static_cast<U64>(sq), "White bitboard does not match FEN string.");
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'B':
                StateException::Check(bitBoard.Pieces[static_cast<int>(EPiece::W_Bishop)] & static_cast<U64>(sq), "White Bishop bitboard does not match FEN string.");
                StateException::Check(!(bitBoard.Pieces[static_cast<int>(EPiece::Empty)] & static_cast<U64>(sq)), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(bitBoard.PiecesByColor[static_cast<int>(EColor::White)] & static_cast<U64>(sq), "White bitboard does not match FEN string.");
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'Q':
                StateException::Check(bitBoard.Pieces[static_cast<int>(EPiece::W_Queen)] & static_cast<U64>(sq), "White Queen bitboard does not match FEN string.");
                StateException::Check(!(bitBoard.Pieces[static_cast<int>(EPiece::Empty)] & static_cast<U64>(sq)), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(bitBoard.PiecesByColor[static_cast<int>(EColor::White)] & static_cast<U64>(sq), "White bitboard does not match FEN string.");
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'K':
                StateException::Check(bitBoard.Pieces[static_cast<int>(EPiece::W_King)] & static_cast<U64>(sq), "White King bitboard does not match FEN string.");
                StateException::Check(!(bitBoard.Pieces[static_cast<int>(EPiece::Empty)] & static_cast<U64>(sq)), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(bitBoard.PiecesByColor[static_cast<int>(EColor::White)] & static_cast<U64>(sq), "White bitboard does not match FEN string.");
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case 'P':
                StateException::Check(bitBoard.Pieces[static_cast<int>(EPiece::W_Pawn)] & static_cast<U64>(sq), "White Pawn bitboard does not match FEN string.");
                StateException::Check(!(bitBoard.Pieces[static_cast<int>(EPiece::Empty)] & static_cast<U64>(sq)), "Empty bitboard reports non-empty square as empty.");
                StateException::Check(bitBoard.PiecesByColor[static_cast<int>(EColor::White)] & static_cast<U64>(sq), "White bitboard does not match FEN string.");
                sq = static_cast<U64>(sq) & FILE_H ? sq - 7 : sq + 1;
                break;
            case '/':
                sq = sq - 8;
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
                const ESquare end = sq + (j - 1);

                for (; sq <= end; ++sq)
                {
                    StateException::Check(bitBoard.Pieces[static_cast<int>(EPiece::Empty)] & static_cast<U64>(sq), "Bitboard reported as not empty for empty FEN square.");
                    StateException::Check(!(bitBoard.PiecesByColor[static_cast<int>(EColor::Black)] & static_cast<U64>(sq)), "Black bitboard reported occupied for an empty FEN square.");
                    StateException::Check(!(bitBoard.PiecesByColor[static_cast<int>(EColor::White)] & static_cast<U64>(sq)), "White bitboard reported occupied for an empty FEN square.");
                    for (EPiece p = EPiece::W_Pawn; p != EPiece::Invalid; ++p)
                    {
                        std::ostringstream msg;
                        msg << "Bitboard index (" << static_cast<int>(p) << ") reported occupied for an empty FEN square";
                        StateException::Check(!(bitBoard.Pieces[static_cast<int>(p)] & static_cast<U64>(sq)), msg.str().c_str());
                    }
                }

                sq = static_cast<U64>(end) & FILE_H ? end - 7 : sq;
            }
                break;

            default:
            {
                std::string msg = std::string("Unrecognized token in FEN string.") + std::string ({'(', token, ')'});
                throw std::invalid_argument(msg.c_str());
            }
        }
    }

    fenStream >> token;
    if (token == 'w')
    {
        StateException::Check(sideToMove == EColor::White, "Wrong side to move reported (Expected White)");
    }
    else if (token == 'b')
    {
        StateException::Check(sideToMove == EColor::Black, "Wrong side to move reported (Exptected Black)");
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
    ESquare enPassant = ESquare::ER;
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

        enPassant = static_cast<ESquare>(1ULL << ((rank - 1) * 8 + file));
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
    ESquare from = ESquare::ER;
    ESquare to = ESquare::ER;
    EPiece movedType = EPiece::Invalid;
    EPiece capturedType = EPiece::Invalid;
    EPiece promotedType = EPiece::Invalid;
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
    from = static_cast<ESquare>(1ULL << ((rank - 1) * 8 + file));

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
    to = static_cast<ESquare>(1ULL << ((rank - 1) * 8 + file));

    const int pieceIdx = sideToMove == EColor::White ? static_cast<int>(EPiece::W_Pawn) :
        static_cast<int>(EPiece::B_Pawn);

    if (bitBoard.Pieces[pieceIdx] & static_cast<U64>(from) && to == StateHistory.back().EnpassantSquare)
    {
        capturedType = sideToMove == EColor::White ? EPiece::B_Pawn : EPiece::B_Pawn;
    }
    else if (bitBoard.Pieces[static_cast<int>(EPiece::Empty)] & static_cast<U64>(to))
    {
        for (EPiece p = EPiece::W_Pawn; p != EPiece::Invalid; ++p)
        {
            if (bitBoard.Pieces[static_cast<int>(p)] & static_cast<U64>(to))
            {
                capturedType = p;
                break;
            }
        }
    }

    for (EPiece p = EPiece::W_Pawn; p != EPiece::Invalid; ++p)
    {
        if (bitBoard.Pieces[static_cast<int>(p)] & static_cast<U64>(from))
        {
            movedType = p;
            break;
        }
    }

    switch (str.back())
    {
        case 'q':
            promotedType = sideToMove == EColor::White ? EPiece::W_Queen : EPiece::B_Queen;
            break;
        case 'r':
            promotedType = sideToMove == EColor::White ? EPiece::W_Rook : EPiece::B_Rook;
            break;
        case 'b':
            promotedType = sideToMove == EColor::White ? EPiece::W_Bishop : EPiece::B_Bishop;
            break;
        case 'k':
            promotedType = sideToMove == EColor::White ? EPiece::W_Knight : EPiece::B_Knight;
            break;
    }

    if (sideToMove == EColor::White)
    {
        kingCastle = movedType == EPiece::W_King
        && from == ESquare::E1
        && to == ESquare::G1;

        queenCastle = movedType == EPiece::W_King
        && from == ESquare::E1
        && to == ESquare::C1;
    }
    else if (sideToMove == EColor::Black)
    {
        kingCastle = movedType == EPiece::B_King
        && from == ESquare::E8
        && to == ESquare::G8;

        queenCastle = movedType == EPiece::B_King
        && from == ESquare::E8
        && to == ESquare::C8;
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

    for (EPiece p = EPiece::W_Pawn; p != EPiece::Invalid; ++p)
    {

    }

    return moves;
}

void Arktos::Board::MakeMove(Move move)
{
    const U64 dest = move.GetTo();
    const U64 start = move.GetFrom();
    const EPiece movedType = move.GetMovedPiece();
    const EPiece capturedType = move.GetCapturedPiece();
    const EPiece promotedType = move.GetPromotedPiece();
    BoardState newState(StateHistory.back());

    /*
     * See if the other side is in check
     */

    assert(sideToMove != EColor::None);
    bitBoard.Pieces[static_cast<int>(movedType)] ^= start | dest;
    bitBoard.Pieces[static_cast<int>(EPiece::Empty)] ^= start | dest;
    bitBoard.PiecesByColor[static_cast<int>(sideToMove)] ^= start | dest;

    if (sideToMove == EColor::White &&
        movedType == EPiece::W_Pawn &&
        capturedType == EPiece::B_Pawn &&
        static_cast<ESquare>(dest) == StateHistory.back().EnpassantSquare)
    {
        bitBoard.Pieces[static_cast<int>(EPiece::B_Pawn)] ^= dest - 8;
        bitBoard.PiecesByColor[static_cast<int>(EColor::Black)] ^= dest - 8;
        newState.EnpassantSquare = ESquare::ER;
    }
    else if (sideToMove == EColor::Black &&
            movedType == EPiece::B_Pawn &&
            capturedType == EPiece::W_Pawn &&
            static_cast<ESquare>(dest) == StateHistory.back().EnpassantSquare)
    {
        bitBoard.Pieces[static_cast<int>(EPiece::W_Pawn)] ^= dest + 8;
        bitBoard.PiecesByColor[static_cast<int>(EColor::White)] ^= dest + 8;
        newState.EnpassantSquare = ESquare::ER;
    }
    else if (capturedType != EPiece::Invalid)
    {
        bitBoard.Pieces[static_cast<int>(capturedType)] ^= dest;
        bitBoard.PiecesByColor[static_cast<int>(sideToMove == EColor::White ? EColor::Black : EColor::White)] ^= dest;
    }

    if (move.KingSideCastle())
    {
        if (sideToMove == EColor::White)
        {
            bitBoard.Pieces[static_cast<int>(EPiece::W_Rook)] ^= static_cast<U64>(ESquare::H1) | static_cast<U64>(ESquare::F1);
            bitBoard.PiecesByColor[static_cast<int>(EColor::White)] ^= static_cast<U64>(ESquare::H1) | static_cast<U64>(ESquare::F1);
            newState.CastlingRights &= k_CAST | q_CAST;
        }
        else
        {
            bitBoard.Pieces[static_cast<int>(EPiece::B_Rook)] ^= static_cast<U64>(ESquare::H8) | static_cast<U64>(ESquare::F8);
            bitBoard.PiecesByColor[static_cast<int>(EColor::Black)] ^= static_cast<U64>(ESquare::H8) | static_cast<U64>(ESquare::F8);
            newState.CastlingRights &= K_CAST | Q_CAST;
        }
    }
    else if (move.QueenSideCastle())
    {
        if (sideToMove == EColor::White)
        {
            bitBoard.Pieces[static_cast<int>(EPiece::W_Rook)] ^= static_cast<U64>(ESquare::A1) | static_cast<U64>(ESquare::D1);
            bitBoard.PiecesByColor[static_cast<int>(EColor::White)] ^= static_cast<U64>(ESquare::A1) | static_cast<U64>(ESquare::D1);
            newState.CastlingRights &= k_CAST | q_CAST;
        }
        else
        {
            bitBoard.Pieces[static_cast<int>(EPiece::B_Rook)] ^= static_cast<U64>(ESquare::A8) | static_cast<U64>(ESquare::D8);
            bitBoard.PiecesByColor[static_cast<int>(EColor::Black)] ^= static_cast<U64>(ESquare::A8) | static_cast<U64>(ESquare::D8);
            newState.CastlingRights &= K_CAST | Q_CAST;
        }
    }

    if (movedType == EPiece::W_Pawn || movedType == EPiece::B_Pawn)
    {
        if (promotedType != EPiece::Invalid)
        {
            bitBoard.Pieces[static_cast<int>(movedType)] ^= dest;
            bitBoard.Pieces[static_cast<int>(promotedType)] ^= dest;
            newState.EnpassantSquare = ESquare::ER;
        }
        else if (movedType == EPiece::W_Pawn && start & RANK_2 && dest & RANK_4)
        {
            newState.EnpassantSquare = static_cast<ESquare>(dest >> 8);
        }
        else if (movedType == EPiece::B_Pawn && start & RANK_7 && dest & RANK_5)
        {
            newState.EnpassantSquare = static_cast<ESquare>(dest << 8);
        }
        else
        {
            newState.EnpassantSquare = ESquare::ER;
        }
    }
    else
    {
        newState.EnpassantSquare = ESquare::ER;
    }

    if (capturedType != EPiece::Invalid || movedType == EPiece::W_Pawn || movedType == EPiece::B_Pawn)
    {
        newState.Repetitions = 0;
    }
    else
    {
        newState.Repetitions++;
    }

    if (sideToMove == EColor::Black)
    {
        fullMoveClock++;
    }

    sideToMove = sideToMove == EColor::White ? EColor::Black : EColor::White;

    StateHistory.push_back(newState);
}

void Arktos::Board::UnMakeMove(Move move)
{

}

std::string Arktos::Board::GetBitBoardStr(EPiece type) const
{
    constexpr U64 bit = 1LL << 63;
    const U64 board = bitBoard.Pieces[static_cast<int>(type)];
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
