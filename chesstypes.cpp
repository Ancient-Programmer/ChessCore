#include "chesstypes.h"


namespace  ChessCore
{

void Hash_128::clear() noexcept
{
    *this = Hash_128{};
}

bool Hash_128::operator==(const Hash_128& other) const noexcept
{
    return m_low == other.m_low && m_high == other.m_high;
}

Hash_128::Hash_128(uint64_t high, uint64_t low) noexcept
{
    m_low = low;
    m_high = high;
}

Piece::Piece(PieceType type, PieceColour colour) noexcept
{
    m_type = type;
    m_colour = colour;
}

void Piece::clear() noexcept
{
    *this = Piece{};
}

bool Piece::isEmpty() const noexcept
{
    return m_type == PieceType::None || m_colour == PieceColour::None;
}

bool Piece::isNotEmpty() const noexcept
{
    return !isEmpty();
}

bool Piece::operator==(const Piece& other) const noexcept
{
    return m_type == other.m_type && m_colour == other.m_colour;
}

bool Piece::operator!=(const Piece& other) const noexcept
{
    return !(*this == other);
}

Move::Move(int32_t from, int32_t to, Piece promo) noexcept
{
    m_from = from;
    m_to = to;
    m_promo = promo;
}

void Move::clear() noexcept
{
    *this = Move{};
}

bool Move::isEmpty() const noexcept
{
    return m_from < 0 || m_to < 0;
}

bool Move::isNotEmpty() const noexcept
{
    return !isEmpty();
}

bool Move::operator==(const Move& other) const noexcept
{
    return m_from  == other.m_from &&
           m_to    == other.m_to &&
           m_promo == other.m_promo;
}

bool Move::operator!=(const Move& other) const noexcept
{
    return !(*this == other);
}

int32_t UndoMove::getFullMove() const
{
    return m_fullMove;
}

void UndoMove::setFullMove(int32_t fullMove)
{
    m_fullMove = fullMove;
}

void UndoMove::clear() noexcept
{
   *this = UndoMove{};
}

uint8_t toU8(CastlingRight right)
{
    return static_cast<uint8_t>(right);
}

CastlingRight operator|(CastlingRight lhs, CastlingRight rhs)
{
    return static_cast<CastlingRight>(toU8(lhs) | toU8(rhs));
}

CastlingRight operator&(CastlingRight lhs, CastlingRight rhs)
{
    return static_cast<CastlingRight>(toU8(lhs) & toU8(rhs));
}

CastlingRight &operator|=(CastlingRight &lhs, CastlingRight rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

bool hasFlag(CastlingRight rights, CastlingRight flag)
{
    return (rights & flag) != CastlingRight::None;
}

char pieceTypeToChar(PieceType type)
{
    switch (type) {
    case PieceType::Pawn:   return 'P';
    case PieceType::Knight: return 'N';
    case PieceType::Bishop: return 'B';
    case PieceType::Rook:   return 'R';
    case PieceType::Queen:  return 'Q';
    case PieceType::King:   return 'K';
    default:                return 'X';
    }
}

char pieceColourToChar(PieceColour clr)
{
    switch (clr) {
    case PieceColour::Black: return 'B';
    case PieceColour::White: return 'W';
    default:                 return 'X';
    }
}

const char *pieceToStr(const Piece &p)
{
    static constexpr const char* Table[PieceTypeCount][PieceColourCount] =
        {
            { "PW", "PB" }, // Pawn
            { "NW", "NB" }, // Knight
            { "BW", "BB" }, // Bishop
            { "RW", "RB" }, // Rook
            { "QW", "QB" }, // Queen
            { "KW", "KB" }  // King
        };

    const uint32_t typeIndex = static_cast<uint32_t>(p.m_type);
    const uint32_t colourIndex = static_cast<uint32_t>(p.m_colour);

    if (typeIndex >= PieceTypeCount || colourIndex >= PieceColourCount)
    {
        return "XX";
    }
    return Table[typeIndex][colourIndex];
}


} // namespace ChessCore

