#include "bitboard.h"
#include "bitutils.h"
#include "zobrist.h"
#include "uciutils.h"



namespace ChessCore
{

// -----------------------------
// Constructors
// -----------------------------

uint64_t BitBoard::getWhiteOccupancy() const noexcept
{
    return m_whiteOccupancy;
}

void BitBoard::setWhiteOccupancy(uint64_t whiteOccupancy) noexcept
{
    m_whiteOccupancy = whiteOccupancy;
}


uint64_t BitBoard::getBlackOccupancy() const noexcept
{
    return m_blackOccupancy;
}

void BitBoard::setBlackOccupancy(uint64_t blackOccupancy) noexcept
{
    m_blackOccupancy = blackOccupancy;
}

uint64_t BitBoard::getEmptySquares() const noexcept
{
    return ~m_occupancy;
}

BitBoard::BitBoard( const uint64_t (&bitboards)[PieceTypeCount][PieceColourCount],
                    CastlingRight hasCastling,
                    int32_t enPassSq,
                    PieceColour mover,
                    int32_t halfMove,
                    int32_t fullMove,
                    Hash_128 hash) noexcept
                    : m_castlingRights(hasCastling),
                    m_enPassantSquare(enPassSq),
                    m_moverColour(mover),
                    m_halfMoveClock(halfMove),
                    m_fullMove(fullMove),
                    m_hash(hash)
{

    for (uint32_t piece = 0; piece < PieceTypeCount; ++piece)
    {
        for (uint32_t colour = 0; colour < PieceColourCount; ++colour)
        {
            m_bitBoards[piece][colour] = bitboards[piece][colour];
        }
    }
    updateOccupancy();
}


bool BitBoard::fromFEN(const std::string& fen)
{
    uint64_t newBitBoards[PieceTypeCount][PieceColourCount] = {};

    PieceColour newMoverColour = PieceColour::None;
    CastlingRight newCastlingRights = CastlingRight::None;
    int32_t newEnPassantSquare = BitUtils::Invalid_Square;
    int32_t newHalfMoveClock = 0;
    int32_t newFullMove = 1;

    if (!UciUtils::fenToBitboards(
            fen,
            newBitBoards,
            newMoverColour,
            newCastlingRights,
            newEnPassantSquare,
            newHalfMoveClock,
            newFullMove))
    {
        return false;
    }

    clear();

    for (uint32_t type = 0; type < PieceTypeCount; ++type)
    {
        for (uint32_t colour = 0; colour < PieceColourCount; ++colour)
        {
            m_bitBoards[type][colour] = newBitBoards[type][colour];
        }
    }

    m_moverColour = newMoverColour;
    m_castlingRights = newCastlingRights;
    m_enPassantSquare = newEnPassantSquare;
    m_halfMoveClock = newHalfMoveClock;
    m_fullMove = newFullMove;

    updateOccupancy();
    updateHash();

    return true;
}


std::string BitBoard::toFEN() const
{
    return UciUtils::bitboardsToFen(m_bitBoards,
                              m_moverColour,
                              m_castlingRights,
                              m_enPassantSquare,
                              m_halfMoveClock,
                              m_fullMove);
}



const uint64_t (&BitBoard::getAllBitBoards() const noexcept)
    [PieceTypeCount][PieceColourCount]
{
    return m_bitBoards;
}

// -----------------------------
// Mover
// -----------------------------

PieceColour BitBoard::getMoversColour() const noexcept
{
    return m_moverColour;
}

void BitBoard::setMoversColour(PieceColour colour) noexcept
{
    if (colour == PieceColour::None)
    {
        return;
    }

    m_moverColour = colour;
    updateHash();
}

void BitBoard::toggleMover() noexcept
{
    switch (m_moverColour)
    {
    case PieceColour::White:
        m_moverColour = PieceColour::Black;
        break;

    case PieceColour::Black:
        m_moverColour = PieceColour::White;
        break;

    default:
        // Keep as-is, or reset/log in debug.
        break;
    }
}




// -----------------------------
// Occupancy
// -----------------------------

void BitBoard::updateOccupancy() noexcept
{
    clearOccupancy();

    for (uint32_t piece = 0; piece < PieceTypeCount; ++piece)
    {
        m_whiteOccupancy |= m_bitBoards[piece][WhitePieceColourIndex];
        m_blackOccupancy |= m_bitBoards[piece][BlackPieceColourIndex];
    }
    m_occupancy = m_whiteOccupancy | m_blackOccupancy;
}

void BitBoard::clearOccupancy() noexcept
{
    m_whiteOccupancy = 0ULL;
    m_blackOccupancy = 0ULL;
    m_occupancy = 0ULL;
}

uint64_t BitBoard::getOccupancy() const noexcept
{
    return m_occupancy;
}

void BitBoard::setOccupancy(uint64_t occupancy) noexcept
{
    m_occupancy = occupancy;
    updateHash();
}


// -----------------------------
// Piece access
// -----------------------------


Piece BitBoard::getPieceAt(int32_t square) const noexcept
{
    if(!BitUtils::isValidSquare(square))
    {
        return Piece();
    }

    const uint64_t mask = BitUtils::bitMask(square);

    if ((m_occupancy & mask) == 0ULL)
    {
        return Piece();
    }

    for (uint32_t piece = 0; piece < PieceTypeCount; ++piece)
    {
        if ((m_bitBoards[piece][WhitePieceColourIndex] & mask) != 0ULL)
        {
            return Piece(static_cast<PieceType>(piece), PieceColour::White);
        }

        if ((m_bitBoards[piece][BlackPieceColourIndex] & mask) != 0ULL)
        {
            return Piece(static_cast<PieceType>(piece), PieceColour::Black);
        }
    }
    return Piece();
}

void BitBoard::setPieceAt(const Piece& piece, int32_t square, bool hashUpdate) noexcept
{
    if (!BitUtils::isValidSquare(square) || piece.isEmpty())
    {
        return;
    }

    if(piece.m_colour == PieceColour::None || piece.m_type == PieceType::None)
    {
        return;
    }

    clearSquareAt(square, false);

    const uint64_t mask = BitUtils::bitMask(square);

    const uint32_t typeIndex =
        static_cast<uint32_t>(piece.m_type);

    const uint32_t colourIndex =
        static_cast<uint32_t>(piece.m_colour);

    m_bitBoards[typeIndex][colourIndex] |= mask;

    if (piece.m_colour == PieceColour::White)
    {
        m_whiteOccupancy |= mask;
    }
    else if (piece.m_colour == PieceColour::Black)
    {
        m_blackOccupancy |= mask;
    }

    m_occupancy |= mask;

    if (hashUpdate)
    {
        updateHash();
    }
}

void BitBoard::clearSquareAt(int32_t square, bool hashUpdate) noexcept
{
    if (!BitUtils::isValidSquare(square))
    {
        return;
    }

    const uint64_t clearMask = ~BitUtils::bitMask(square);

    for (uint32_t piece = 0; piece < PieceTypeCount; ++piece)
    {
        m_bitBoards[piece][WhitePieceColourIndex] &= clearMask;
        m_bitBoards[piece][BlackPieceColourIndex] &= clearMask;
    }

    m_whiteOccupancy &= clearMask;
    m_blackOccupancy &= clearMask;
    m_occupancy      &= clearMask;

    if(hashUpdate)
    {
        updateHash();
    }
}

Piece BitBoard::popPieceAt(int32_t square, bool updateHash) noexcept
{
    if (!BitUtils::isValidSquare(square))
    {
        return Piece{};
    }

    Piece returned = getPieceAt(square);

    if (returned.isNotEmpty())
    {
        clearSquareAt(square, updateHash);
    }

    return returned;
}

MoveType BitBoard::movePiece(int32_t from, int32_t to) noexcept
{
    if (!BitUtils::isValidSquare(from) || !BitUtils::isValidSquare(to))
    {
        return MoveType::Invalid;
    }

    if (from == to)
    {
        return MoveType::Invalid;
    }

    Piece src = getPieceAt(from);
    Piece dst = getPieceAt(to);

    if (src.isEmpty())
    {
        return MoveType::Invalid;
    }

    if (dst.isNotEmpty() && src.m_colour == dst.m_colour)
    {
        return MoveType::Invalid;
    }

    const MoveType moveType = dst.isEmpty()
                                  ? MoveType::Normal
                                  : MoveType::Capture;

    setPieceAt(src, to, false);
    clearSquareAt(from);

    return moveType;
}


// -----------------------------
// Board state
// -----------------------------

void BitBoard::clear() noexcept
{
    *this = BitBoard{};
}

void BitBoard::init() noexcept
{
    clear();

    m_bitBoards[RookIndex][WhitePieceColourIndex]   = BitUtils::bitMask(0) | BitUtils::bitMask(7);
    m_bitBoards[KnightIndex][WhitePieceColourIndex] = BitUtils::bitMask(1) | BitUtils::bitMask(6);
    m_bitBoards[BishopIndex][WhitePieceColourIndex] = BitUtils::bitMask(2) | BitUtils::bitMask(5);
    m_bitBoards[QueenIndex][WhitePieceColourIndex]  = BitUtils::bitMask(3);

    m_bitBoards[KingIndex][WhitePieceColourIndex]   = BitUtils::bitMask(4);
    m_bitBoards[PawnIndex][WhitePieceColourIndex]   = 0xFFULL << 8;


    m_bitBoards[RookIndex][BlackPieceColourIndex]   = BitUtils::bitMask(56) | BitUtils::bitMask(63);
    m_bitBoards[KnightIndex][BlackPieceColourIndex] = BitUtils::bitMask(57) | BitUtils::bitMask(62);

    m_bitBoards[BishopIndex][BlackPieceColourIndex] = BitUtils::bitMask(58) | BitUtils::bitMask(61);
    m_bitBoards[QueenIndex][BlackPieceColourIndex]  = BitUtils::bitMask(59);

    m_bitBoards[KingIndex][BlackPieceColourIndex]   = BitUtils::bitMask(60);
    m_bitBoards[PawnIndex][BlackPieceColourIndex]   = 0xFFULL << 48;

    // -----------------------------
    // Game state
    // -----------------------------

    m_castlingRights = CastlingRight::WhiteKingSide  |
                       CastlingRight::WhiteQueenSide |
                       CastlingRight::BlackKingSide  |
                       CastlingRight::BlackQueenSide;

    m_enPassantSquare = BitUtils::Invalid_Square;
    m_moverColour = PieceColour::White;

    m_halfMoveClock = 0;
    m_fullMove = 1;

    updateOccupancy();
    updateHash();
}

// -----------------------------
// Castling
// -----------------------------

CastlingRight BitBoard::getCastling() const noexcept
{
    return m_castlingRights;
}

void BitBoard::setCastling(CastlingRight castingRights) noexcept
{
    m_castlingRights = castingRights;
    updateHash();
}

bool BitBoard::hasCastlingRight(CastlingRight right) const noexcept
{
    return hasFlag(m_castlingRights, right);
}

void BitBoard::addCastlingRight(CastlingRight right) noexcept
{
    m_castlingRights |= right;
    updateHash();
}

void BitBoard::removeCastlingRight(CastlingRight right) noexcept
{
    const uint8_t newRights =
        static_cast<uint8_t>(toU8(m_castlingRights) & ~toU8(right));

    m_castlingRights = static_cast<CastlingRight>(newRights);

    updateHash();
}


// -----------------------------
// Bitboard access
// -----------------------------

uint64_t BitBoard::occupiedSquares() const noexcept
{
    uint64_t occupied = 0ULL;

    for (uint32_t piece = 0; piece < PieceTypeCount; ++piece)
    {
        occupied |= m_bitBoards[piece][WhitePieceColourIndex];
        occupied |= m_bitBoards[piece][BlackPieceColourIndex];
    }
    return occupied;
}

uint64_t BitBoard::getBitBoard(PieceType type, PieceColour colour) const noexcept
{
    if(type == PieceType::None || colour == PieceColour::None)
    {
        return 0ULL;
    }
    return m_bitBoards[static_cast<uint32_t>(type)][static_cast<uint32_t>(colour)];
}

uint64_t* BitBoard::getBitBoardPtr(PieceType type, PieceColour colour) noexcept
{
    if (type == PieceType::None || colour == PieceColour::None)
        return nullptr;

    return &m_bitBoards[static_cast<uint32_t>(type)]
                       [static_cast<uint32_t>(colour)];
}

void BitBoard::setBitBoard(uint64_t bitBoard, PieceType type, PieceColour colour) noexcept
{
    if(type == PieceType::None || colour == PieceColour::None)
    {
        return;
    }
    m_bitBoards[static_cast<uint32_t>(type)][static_cast<uint32_t>(colour)] = bitBoard;
    updateOccupancy();
    updateHash();
}

// -----------------------------
// En passant
// -----------------------------

int32_t BitBoard::getEnPassantSquare() const noexcept
{
    return m_enPassantSquare;
}

void BitBoard::setEnPassantSquare(int32_t square) noexcept
{
    if(square < BitUtils::Invalid_Square || square > BitUtils::Max_Square)
    {
        return;
    }
    m_enPassantSquare = square;
    updateHash();
}


// -----------------------------
// Move counters
// -----------------------------

int32_t BitBoard::getFullMove() const noexcept
{
    return m_fullMove;
}

void BitBoard::setFullMove(int32_t fullMove) noexcept
{
    if (fullMove < 1)
    {
        return;
    }
    m_fullMove = fullMove;
}

int32_t BitBoard::getHalfMoveClock() const noexcept
{
    return m_halfMoveClock;
}

void BitBoard::setHalfMoveClock(int32_t halfMoveClock) noexcept
{
    if (halfMoveClock < 0)
    {
        return;
    }
    m_halfMoveClock = halfMoveClock;
}

// -----------------------------
// Hash getter/setter
// -----------------------------
Hash_128 BitBoard::getHash() const noexcept
{
    return m_hash;
}

void BitBoard::setHash(const Hash_128& hash) noexcept
{
    m_hash = hash;
}

// -----------------------------
// Local helpers / Zobrist
// -----------------------------

static uint64_t mix64(uint64_t x) noexcept
{
    x ^= x >> 30;
    x *= 0xBF58476D1CE4E5B9ULL;
    x ^= x >> 27;
    x *= 0x94D049BB133111EBULL;
    x ^= x >> 31;
    return x;
}

// -----------------------------
// Zobrist hash
// -----------------------------


Hash_128 BitBoard::computeHash() const noexcept
{
    uint64_t low = 0ULL;

    // -----------------------------
    // Pieces
    // -----------------------------

    for (uint32_t piece = 0; piece < PieceTypeCount; ++piece)
    {
        for (uint32_t colour = 0; colour < PieceColourCount; ++colour)
        {
            uint64_t bb = m_bitBoards[piece][colour];

            while (bb != 0ULL)
            {
                const int32_t square = BitUtils::popLSB(&bb);

                if (BitUtils::isValidSquare(square))
                {
                    low ^= Zobrist::PieceSquare[piece][colour][square];
                }
            }
        }
    }

    // -----------------------------
    // Side to move
    // -----------------------------

    if (m_moverColour == PieceColour::Black)
    {
        low ^= Zobrist::SideToMove;
    }

    // -----------------------------
    // Castling rights
    // -----------------------------

    const uint8_t castlingIndex =
        static_cast<uint8_t>(toU8(m_castlingRights) & 0x0F);

    low ^= Zobrist::Castling[castlingIndex];

    // -----------------------------
    // En passant
    // -----------------------------

    if (BitUtils::isValidSquare(m_enPassantSquare))
    {
        const uint32_t file =
            static_cast<uint32_t>(m_enPassantSquare % 8);

        low ^= Zobrist::EnPassantFile[file];
    }

    Hash_128 result;
    result.m_low = low;
    result.m_high = mix64(low ^ 0x9E3779B97F4A7C15ULL);

    return result;
}

void BitBoard::updateHash() noexcept
{
    m_hash = computeHash();
}

const Hash_128& BitBoard::hash() const noexcept
{
    return m_hash;
}


// =================================
// FEN / UCI / HELPERS
// =================================






} // namespace ChessCore
