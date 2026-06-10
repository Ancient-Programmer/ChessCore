#include "uciutils.h"
#include "bitutils.h"



#include <cassert>
#include <cstdint>
#include <string>
#include <sstream>



namespace ChessCore
{


static constexpr uint32_t pt(PieceType type) noexcept
{
    return static_cast<uint32_t>(type);
}

static constexpr uint32_t pc(PieceColour colour) noexcept
{
    return static_cast<uint32_t>(colour);
}

static char pieceAtFromBitboards(
    int32_t square,
    const uint64_t (&bitboards)[PieceTypeCount][PieceColourCount]) noexcept
{
    const uint64_t mask = BitUtils::bitMask(square);

    if (mask == 0ULL)
        return '\0';

    if ((bitboards[pt(PieceType::Pawn)]  [pc(PieceColour::White)] & mask) != 0ULL) return 'P';
    if ((bitboards[pt(PieceType::Knight)][pc(PieceColour::White)] & mask) != 0ULL) return 'N';
    if ((bitboards[pt(PieceType::Bishop)][pc(PieceColour::White)] & mask) != 0ULL) return 'B';
    if ((bitboards[pt(PieceType::Rook)]  [pc(PieceColour::White)] & mask) != 0ULL) return 'R';
    if ((bitboards[pt(PieceType::Queen)] [pc(PieceColour::White)] & mask) != 0ULL) return 'Q';
    if ((bitboards[pt(PieceType::King)]  [pc(PieceColour::White)] & mask) != 0ULL) return 'K';

    if ((bitboards[pt(PieceType::Pawn)]  [pc(PieceColour::Black)] & mask) != 0ULL) return 'p';
    if ((bitboards[pt(PieceType::Knight)][pc(PieceColour::Black)] & mask) != 0ULL) return 'n';
    if ((bitboards[pt(PieceType::Bishop)][pc(PieceColour::Black)] & mask) != 0ULL) return 'b';
    if ((bitboards[pt(PieceType::Rook)]  [pc(PieceColour::Black)] & mask) != 0ULL) return 'r';
    if ((bitboards[pt(PieceType::Queen)] [pc(PieceColour::Black)] & mask) != 0ULL) return 'q';
    if ((bitboards[pt(PieceType::King)]  [pc(PieceColour::Black)] & mask) != 0ULL) return 'k';

    return '\0';
}

static void piecePlacementToFen(
    const uint64_t (&bitboards)[PieceTypeCount][PieceColourCount],
    std::string& fen)
{
    for (int32_t rank = BitUtils::Max_Rank;
         rank >= BitUtils::Min_Rank;
         --rank)
    {
        int32_t emptyCount = 0;

        for (int32_t file = BitUtils::Min_File;
             file <= BitUtils::Max_File;
             ++file)
        {
            const int32_t square = BitUtils::toSquare(rank, file);
            const char pieceChar = pieceAtFromBitboards(square, bitboards);

            if (pieceChar == '\0')
            {
                ++emptyCount;
                continue;
            }

            if (emptyCount > 0)
            {
                fen += static_cast<char>('0' + emptyCount);
                emptyCount = 0;
            }

            fen += pieceChar;
        }

        if (emptyCount > 0)
        {
            fen += static_cast<char>('0' + emptyCount);
        }

        if (rank > BitUtils::Min_Rank)
        {
            fen += '/';
        }
    }
}

static void sideToMoveToFen(PieceColour moverColour, std::string& fen)
{
#ifndef NDEBUG
    assert(moverColour == PieceColour::White ||
           moverColour == PieceColour::Black);
#endif

    if (moverColour == PieceColour::White)
    {
        fen += 'w';
    }
    else if (moverColour == PieceColour::Black)
    {
        fen += 'b';
    }
    else
    {
        fen += 'w'; // safe fallback
    }
}

static void castlingToFen(CastlingRight castlingRights, std::string& fen)
{
    bool hasCastling = false;

    if (hasFlag(castlingRights, CastlingRight::WhiteKingSide))
    {
        fen += 'K';
        hasCastling = true;
    }

    if (hasFlag(castlingRights, CastlingRight::WhiteQueenSide))
    {
        fen += 'Q';
        hasCastling = true;
    }

    if (hasFlag(castlingRights, CastlingRight::BlackKingSide))
    {
        fen += 'k';
        hasCastling = true;
    }

    if (hasFlag(castlingRights, CastlingRight::BlackQueenSide))
    {
        fen += 'q';
        hasCastling = true;
    }

    if (!hasCastling)
    {
        fen += '-';
    }
}

static void enPassantToFen(int32_t enPassantSquare, std::string& fen)
{
    if (!BitUtils::isValidSquare(enPassantSquare))
    {
        fen += '-';
        return;
    }

    const int32_t file = BitUtils::toFile(enPassantSquare);
    const int32_t rank = BitUtils::toRank(enPassantSquare);

    fen += static_cast<char>('a' + file);
    fen += static_cast<char>('1' + rank);
}


static bool fenCharToPiece(char c,
                           PieceType& outType,
                           PieceColour& outColour) noexcept
{
    outColour = std::isupper(static_cast<unsigned char>(c))
    ? PieceColour::White
    : PieceColour::Black;

    const char lower =
        static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

    switch (lower)
    {
    case 'p':
        outType = PieceType::Pawn;
        return true;

    case 'n':
        outType = PieceType::Knight;
        return true;

    case 'b':
        outType = PieceType::Bishop;
        return true;

    case 'r':
        outType = PieceType::Rook;
        return true;

    case 'q':
        outType = PieceType::Queen;
        return true;

    case 'k':
        outType = PieceType::King;
        return true;

    default:
        outType = PieceType::None;
        outColour = PieceColour::None;
        return false;
    }
}

static bool parsePiecePlacement(
    const std::string& placement,
    uint64_t (&bitboards)[PieceTypeCount][PieceColourCount])
{
    for (uint32_t type = 0; type < PieceTypeCount; ++type)
    {
        for (uint32_t colour = 0; colour < PieceColourCount; ++colour)
        {
            bitboards[type][colour] = 0ULL;
        }
    }

    int32_t rank = BitUtils::Max_Rank;
    int32_t file = BitUtils::Min_File;

    for (char c : placement)
    {
        if (c == '/')
        {
            if (file != BitUtils::Board_Dim)
                return false;

            --rank;
            file = BitUtils::Min_File;

            if (rank < BitUtils::Min_Rank)
                return false;

            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(c)))
        {
            const int32_t emptyCount = c - '0';

            if (emptyCount <= 0 || emptyCount > BitUtils::Board_Dim)
                return false;

            file += emptyCount;

            if (file > BitUtils::Board_Dim)
                return false;

            continue;
        }

        if (file > BitUtils::Max_File ||
            rank < BitUtils::Min_Rank)
        {
            return false;
        }

        PieceType type = PieceType::None;
        PieceColour colour = PieceColour::None;

        if (!fenCharToPiece(c, type, colour))
            return false;

        const int32_t square = BitUtils::toSquare(rank, file);

        if (!BitUtils::isValidSquare(square))
            return false;

        bitboards[pt(type)][pc(colour)] |= BitUtils::bitMask(square);

        ++file;
    }

    return rank == BitUtils::Min_Rank &&
           file == BitUtils::Board_Dim;
}


static bool parseSideToMove(const std::string& side,
                            PieceColour& moverColour) noexcept
{
    if (side == "w")
    {
        moverColour = PieceColour::White;
        return true;
    }

    if (side == "b")
    {
        moverColour = PieceColour::Black;
        return true;
    }

    moverColour = PieceColour::None;
    return false;
}

static bool parseCastling(const std::string& castling,
                          CastlingRight& castlingRights) noexcept
{
    castlingRights = CastlingRight::None;

    if (castling == "-")
        return true;

    for (char c : castling)
    {
        switch (c)
        {
        case 'K':
            castlingRights = castlingRights | CastlingRight::WhiteKingSide;
            break;

        case 'Q':
            castlingRights = castlingRights | CastlingRight::WhiteQueenSide;
            break;

        case 'k':
            castlingRights = castlingRights | CastlingRight::BlackKingSide;
            break;

        case 'q':
            castlingRights = castlingRights | CastlingRight::BlackQueenSide;
            break;

        default:
            return false;
        }
    }

    return true;
}

static bool parseEnPassant(const std::string& ep,
                           int32_t& enPassantSquare) noexcept
{
    if (ep == "-")
    {
        enPassantSquare = BitUtils::Invalid_Square;
        return true;
    }

    if (ep.size() != 2)
        return false;

    const char fileChar = ep[0];
    const char rankChar = ep[1];

    if (fileChar < 'a' || fileChar > 'h')
        return false;

    if (rankChar < '1' || rankChar > '8')
        return false;

    const int32_t file = fileChar - 'a';
    const int32_t rank = rankChar - '1';

    const int32_t square = BitUtils::toSquare(rank, file);

    if (!BitUtils::isValidSquare(square))
        return false;

    enPassantSquare = square;
    return true;
}


std::string UciUtils::bitboardsToFen(const uint64_t (&bitboards)[PieceTypeCount][PieceColourCount],
                                      PieceColour moverColour,
                                      CastlingRight castlingRights,
                                      int32_t enPassantSquare,
                                      int32_t halfMoveClock,
                                      int32_t fullMove)
{
    std::string fen;
    fen.reserve(96);

    // Piece placement.
    piecePlacementToFen(bitboards, fen);

    // Side to move.
    fen += ' ';
    sideToMoveToFen(moverColour, fen);

    // Castling rights.
    fen += ' ';
    castlingToFen(castlingRights, fen);

    // En passant square.
    fen += ' ';
    enPassantToFen(enPassantSquare, fen);

    // Halfmove clock.
    fen += ' ';
    fen += std::to_string(halfMoveClock);

    // Fullmove number.
    fen += ' ';
    fen += std::to_string(fullMove);

    return fen;
}


bool UciUtils::fenToBitboards(
    const std::string& fen,
    uint64_t (&bitboards)[PieceTypeCount][PieceColourCount],
    PieceColour& moverColour,
    CastlingRight& castlingRights,
    int32_t& enPassantSquare,
    int32_t& halfMoveClock,
    int32_t& fullMove)
{
    std::istringstream stream(fen);

    std::string placement;
    std::string side;
    std::string castling;
    std::string enPassant;
    std::string halfMoveText;
    std::string fullMoveText;

    if (!(stream >> placement >> side >> castling >> enPassant))
        return false;

    if (!(stream >> halfMoveText))
        halfMoveText = "0";

    if (!(stream >> fullMoveText))
        fullMoveText = "1";

    int32_t parsedHalfMove = 0;
    int32_t parsedFullMove = 1;

    try
    {
        parsedHalfMove = std::stoi(halfMoveText);
        parsedFullMove = std::stoi(fullMoveText);
    }
    catch (...)
    {
        return false;
    }

    if (parsedHalfMove < 0 || parsedFullMove <= 0)
        return false;

    uint64_t parsedBitboards[PieceTypeCount][PieceColourCount] = {};
    PieceColour parsedMover = PieceColour::None;
    CastlingRight parsedCastling = CastlingRight::None;
    int32_t parsedEnPassant = BitUtils::Invalid_Square;

    if (!parsePiecePlacement(placement, parsedBitboards))
        return false;

    if (!parseSideToMove(side, parsedMover))
        return false;

    if (!parseCastling(castling, parsedCastling))
        return false;

    if (!parseEnPassant(enPassant, parsedEnPassant))
        return false;

    for (uint32_t type = 0; type < PieceTypeCount; ++type)
    {
        for (uint32_t colour = 0; colour < PieceColourCount; ++colour)
        {
            bitboards[type][colour] = parsedBitboards[type][colour];
        }
    }

    moverColour = parsedMover;
    castlingRights = parsedCastling;
    enPassantSquare = parsedEnPassant;
    halfMoveClock = parsedHalfMove;
    fullMove = parsedFullMove;

    return true;
}



// UCI



std::string UciUtils::squareToUci(int32_t square)
{
    if (!BitUtils::isValidSquare(square))
        return "";

    const int32_t file = BitUtils::toFile(square);
    const int32_t rank = BitUtils::toRank(square);

    std::string out;
    out += static_cast<char>('a' + file);
    out += static_cast<char>('1' + rank);

    return out;
}

int32_t UciUtils::uciToSquare(char fileChar, char rankChar)
{
    if (fileChar < 'a' || fileChar > 'h')
        return BitUtils::Invalid_Square;

    if (rankChar < '1' || rankChar > '8')
        return BitUtils::Invalid_Square;

    const int32_t file = fileChar - 'a';
    const int32_t rank = rankChar - '1';

    return BitUtils::toSquare(rank, file);
}

static char promotionToUciChar(PieceType type)
{
    switch (type)
    {
    case PieceType::Queen:  return 'q';
    case PieceType::Rook:   return 'r';
    case PieceType::Bishop: return 'b';
    case PieceType::Knight: return 'n';
    default:                return '\0';
    }
}

static PieceType uciCharToPromotion(char c)
{
    switch (static_cast<char>(std::tolower(static_cast<unsigned char>(c))))
    {
    case 'q': return PieceType::Queen;
    case 'r': return PieceType::Rook;
    case 'b': return PieceType::Bishop;
    case 'n': return PieceType::Knight;
    default:  return PieceType::None;
    }
}


std::string UciUtils::moveToUci(const Move& move)
{
    if (!BitUtils::isValidSquare(move.m_from) ||
        !BitUtils::isValidSquare(move.m_to))
    {
        return "";
    }

    std::string uci;

    uci += squareToUci(move.m_from);
    uci += squareToUci(move.m_to);

    if (move.m_promo.isNotEmpty())
    {
        const char promoChar = promotionToUciChar(move.m_promo.m_type);

        if (promoChar != '\0')
            uci += promoChar;
    }

    return uci;
}


Move UciUtils::uciToMove(const std::string& uci)
{
    if (uci.size() != 4 && uci.size() != 5)
        return Move();

    const int32_t from = uciToSquare(uci[0], uci[1]);
    const int32_t to   = uciToSquare(uci[2], uci[3]);

    if (!BitUtils::isValidSquare(from) ||
        !BitUtils::isValidSquare(to))
    {
        return Move();
    }

    Piece promo;

    if (uci.size() == 5)
    {
        const PieceType promoType = uciCharToPromotion(uci[4]);

        if (promoType == PieceType::None)
            return Move();

        /*
         * Colour is unknown from UCI alone.
         * You can fill it later from the moving piece colour.
         */
        promo = Piece(promoType, PieceColour::None);
    }
    return Move(from, to, promo);
}


} // namespace ChessCore


