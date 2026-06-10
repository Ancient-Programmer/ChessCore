#ifndef CHESSTYPES_H
#define CHESSTYPES_H


#include <cstdint>

namespace ChessCore
{

/**
 * @namespace MaxMoves
 * @brief Contains maximum move-count estimates for each piece type.
 *
 * These constants are useful for reserving memory in move lists
 * and avoiding unnecessary dynamic reallocations during move generation.
 */
namespace MaxMoves
{

// -----------------------------
// Constants
// -----------------------------

/**
 * @brief Maximum expected pawn moves.
 */
constexpr uint32_t Pawn = 12;

/**
 * @brief Maximum expected knight moves.
 */
constexpr uint32_t Knight = 8;

/**
 * @brief Maximum expected bishop moves.
 */
constexpr uint32_t Bishop = 13;

/**
 * @brief Maximum expected rook moves.
 */
constexpr uint32_t Rook = 14;

/**
 * @brief Maximum expected queen moves.
 */
constexpr uint32_t Queen = 27;

/**
 * @brief Maximum expected king moves.
 */
constexpr uint32_t King = 10;

/**
 * @brief Default maximum move estimate per piece.
 */
constexpr uint32_t Default_Per_Piece = 27;

/**
 * @brief Maximum estimated move count for one side.
 */
constexpr uint32_t Total_Per_Side = 256;

} // namespace MaxMoves



// -----------------------------
// Enums
// -----------------------------

/**
 * @enum PieceColour
 * @brief Represents the colour of a chess piece.
 */
enum class PieceColour : uint8_t
{
    White,  /**< White piece. */
    Black,  /**< Black piece. */
    None    /**< No colour / empty square. */
};

/**
 * @enum PieceType
 * @brief Represents the type of a chess piece.
 */
enum class PieceType : uint8_t
{
    Pawn,    /**< Pawn piece. */
    Knight,  /**< Knight piece. */
    Bishop,  /**< Bishop piece. */
    Rook,    /**< Rook piece. */
    Queen,   /**< Queen piece. */
    King,    /**< King piece. */
    None     /**< No piece / empty square. */
};


enum class GameStatus : int8_t
{
    Invalid = -1,
    Ongoing = 0,

    WhiteWonByCheckmate,
    BlackWonByCheckmate,

    DrawByStalemate,
    DrawByThreefoldRepetition,
    DrawByFiftyMoveRule,
    DrawByInsufficientMaterial,

    WhiteLostOnTime,
    BlackLostOnTime
};


// -----------------------------
// Helpers
// -----------------------------

char pieceTypeToChar(PieceType type);
char pieceColourToChar(PieceColour clr);


// -----------------------------
// MoveType
// -----------------------------

enum class MoveType: int8_t
{
    Invalid = -1,
    Normal,
    Capture,
    Enpassant,
    Castling,
    Promotion
};


constexpr uint32_t PieceTypeCount = static_cast<uint32_t>(PieceType::None);
constexpr uint32_t PieceColourCount = static_cast<uint32_t>(PieceColour::None);
constexpr uint32_t WhitePieceColourIndex = static_cast<uint32_t>(PieceColour::White);
constexpr uint32_t BlackPieceColourIndex = static_cast<uint32_t>(PieceColour::Black);

constexpr uint32_t PawnIndex   = static_cast<uint32_t>(PieceType::Pawn);
constexpr uint32_t KnightIndex = static_cast<uint32_t>(PieceType::Knight);
constexpr uint32_t BishopIndex = static_cast<uint32_t>(PieceType::Bishop);
constexpr uint32_t RookIndex   = static_cast<uint32_t>(PieceType::Rook);
constexpr uint32_t QueenIndex  = static_cast<uint32_t>(PieceType::Queen);
constexpr uint32_t KingIndex   = static_cast<uint32_t>(PieceType::King);


// -----------------------------
// Castling rights & helpers
// -----------------------------


enum class CastlingRight : uint8_t
{
    None            = 0,
    WhiteKingSide   = 1 << 0,
    WhiteQueenSide  = 1 << 1,
    BlackKingSide   = 1 << 2,
    BlackQueenSide  = 1 << 3
};

uint8_t toU8(CastlingRight right);
CastlingRight operator|(CastlingRight lhs, CastlingRight rhs);
CastlingRight operator&(CastlingRight lhs, CastlingRight rhs);
CastlingRight& operator|=(CastlingRight& lhs, CastlingRight rhs);
bool hasFlag(CastlingRight rights, CastlingRight flag);


// -----------------------------
// Board Hash
// -----------------------------

/**
 * @struct Hash_128
 * @brief Stores a 128-bit board hash.
 *
 * The hash is split into two 64-bit values: high and low.
 * Useful for board comparison, repetition detection, transposition tables,
 * and other engine-level witchcraft humans pretend is “optimization”.
 */
struct Hash_128
{
    /**
     * @brief High 64 bits of the hash.
     */
    uint64_t m_high = 0ULL;

    /**
     * @brief Low 64 bits of the hash.
     */
    uint64_t m_low = 0ULL;

public:

    /**
     * @brief Clears the hash value.
     *
     * Sets both high and low parts to zero.
     */
    void clear() noexcept;
    Hash_128() = default;
    Hash_128(uint64_t high, uint64_t low) noexcept;
    bool operator==(const Hash_128& other) const noexcept;
};


// -----------------------------
// Piece
// -----------------------------

/**
 * @struct Piece
 * @brief Represents a chess piece.
 *
 * A piece is defined by its type and colour.
 * If either value is None, the piece is treated as empty.
 */
struct Piece
{
    /**
     * @brief Type of the piece.
     */
    PieceType m_type = PieceType::None;

    /**
     * @brief Colour of the piece.
     */
    PieceColour m_colour = PieceColour::None;

public:

    /**
     * @brief Creates an empty piece.
     */
    Piece() = default;

    /**
     * @brief Creates a piece with a specific type and colour.
     *
     * @param type The piece type.
     * @param colour The piece colour.
     */
    Piece(PieceType type, PieceColour colour) noexcept;

    /**
     * @brief Clears the piece.
     *
     * Sets both type and colour to None.
     */
    void clear() noexcept;

    /**
     * @brief Checks whether the piece is empty.
     *
     * @return true if the piece has no valid type or colour.
     * @return false otherwise.
     */
    bool isEmpty() const noexcept;

    /**
     * @brief Checks whether the piece is not empty.
     *
     * @return true if the piece has a valid type and colour.
     * @return false otherwise.
     */
    bool isNotEmpty() const noexcept;

    /**
     * @brief Compares two pieces for equality.
     *
     * @param other The piece to compare against.
     * @return true if both pieces have the same type and colour.
     * @return false otherwise.
     */
    bool operator==(const Piece& other) const noexcept;

    /**
     * @brief Compares two pieces for inequality.
     *
     * @param other The piece to compare against.
     * @return true if the pieces are different.
     * @return false otherwise.
     */
    bool operator!=(const Piece& other) const noexcept;
};


/**
     * @brief turns Piece() int const char* str.
     *
     * @param  Piece() struct {type, coour}.
     * @return const char* of type/colour as "PW."
     */
const char* pieceToStr(const Piece& p);

// -----------------------------
// Chess Move
// -----------------------------

/**
 * @class Move
 * @brief Represents a chess move.
 *
 * A move stores the source square, destination square,
 * and an optional promotion piece.
 */
class Move
{
public:

    /**
     * @brief Source square index.
     *
     * Usually expected to be in the range 0-63.
     * A value below 0 means the move is invalid or empty.
     */
    int32_t m_from = -1;

    /**
     * @brief Destination square index.
     *
     * Usually expected to be in the range 0-63.
     * A value below 0 means the move is invalid or empty.
     */
    int32_t m_to = -1;

    /**
     * @brief Promotion piece.
     *
     * Used when a pawn promotes. Empty for normal moves.
     */
    Piece m_promo = {};

public:

    /**
     * @brief Creates an empty move.
     */
    Move() = default;

    /**
     * @brief Creates a move from one square to another.
     *
     * @param from Source square index.
     * @param to Destination square index.
     * @param promo Optional promotion piece.
     */
    Move(int32_t from, int32_t to, Piece promo = {}) noexcept;

    /**
     * @brief Clears the move.
     *
     * Resets source and destination squares and clears promotion data.
     */
    void clear() noexcept;

    /**
     * @brief Checks whether the move is empty.
     *
     * @return true if the move has invalid source or destination squares.
     * @return false otherwise.
     */
    bool isEmpty() const noexcept;

    /**
     * @brief Checks whether the move is not empty.
     *
     * @return false if the move has valid source or destination squares.
     * @return true otherwise.
     */
    bool isNotEmpty() const noexcept;

    /**
     * @brief Compares two moves for equality.
     *
     * @param other The move to compare against.
     * @return true if both moves have the same source, destination, and promotion.
     * @return false otherwise.
     */
    bool operator==(const Move& other) const noexcept;
    /**
     * @brief Compares two moves for equality.
     *
     * @param  other The move to compare against.
     * @return true if both moves have different source, destination, and promotion.
     * @return false otherwise.
     */
    bool operator!=(const Move& other) const noexcept;
};


// -----------------------------
// Undo Chess Move
// -----------------------------

/**
 * @class UndoChessMove
 * @brief Stores enough information to undo a chess move.
 *
 * This class keeps the previous board state needed to safely reverse
 * a move after move generation, search, or actual gameplay.
 *
 * It stores moved pieces, captured pieces, castling rights,
 * en passant state, move counters, rook movement during castling,
 * and the previous board hash.
 */
class UndoMove
{
public:

    /**
     * @brief Original square of the moved piece.
     */
    int32_t m_from = -1;

    /**
     * @brief Destination square of the moved piece.
     */
    int32_t m_to = -1;

    /**
     * @brief Square where a captured piece was located.
     *
     * Usually the same as m_squareTo, except for en passant.
     */
    int32_t m_capturedSquare = -1;

    /**
     * @brief Piece that was moved.
     */
    Piece m_movedPiece = Piece();

    /**
     * @brief Piece that was captured.
     */
    Piece m_capturedPiece = Piece();


    CastlingRight m_castlingRights = CastlingRight::None;

    /**
     * @brief Previous en passant square.
     *
     * A value of -1 means no en passant square was available.
     */
    int32_t m_prevEnPassantSquare = -1;

    /**
     * @brief Colour of the player who moved before the move was applied.
     */
    PieceColour m_prevMover = PieceColour::None;

    /**
     * @brief Original rook square during castling.
     *
     * A value of -1 means no rook movement occurred.
     */
    int32_t m_rookFromSquare = -1;

    /**
     * @brief Destination rook square during castling.
     *
     * A value of -1 means no rook movement occurred.
     */
    int32_t m_rookToSquare = -1;

    /**
     * @brief Rook piece moved during castling.
     */
    Piece m_rookPiece = Piece();

    /**
     * @brief Whether the move was an en passant capture.
     */
    bool m_wasEnPassant = false;

    /**
     * @brief Previous half-move clock.
     *
     * Used for the fifty-move rule.
     */
    int32_t m_halfMoveClock = 0;

    /**
     * @brief Previous full-move number.
     */
    int32_t m_fullMove = 1;

    /**
     * @brief Previous board hash.
     */
    Hash_128 m_boardHash = {};

public:

    /**
     * @brief Clears all undo information.
     *
     * Resets the object to its default empty state.
     */
    void clear() noexcept;

    /**
     * @brief getFullMove
     * @return count full move
     */
    int32_t getFullMove() const;

    /**
     * @brief setFullMove
     * @param newFullMove
     */
    void setFullMove(int32_t newFullMove);
};

} // namespace ChessCore

#endif // CHESSTYPES_H


