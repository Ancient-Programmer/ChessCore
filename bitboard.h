#ifndef BITBOARD_H
#define BITBOARD_H

#include <string>
#include "chesstypes.h"


namespace ChessCore
{

class BitBoard
{

private:
    /**
     * @brief Piece bitboards indexed by piece type and colour.
     *
     * First index:  piece type  0..5
     * Second index: colour      0..1
     *
     * Example:
     * m_bitBoards[PieceType::Pawn][PieceColour::White]
     *
     * The array does not store PieceType::None or PieceColour::None.
     */
    uint64_t m_bitBoards[6][2] = {};

    /**
     * @brief Bitboard containing all white pieces.
     */
    uint64_t m_whiteOccupancy = 0ULL;

    /**
     * @brief Bitboard containing all black pieces.
     */
    uint64_t m_blackOccupancy = 0ULL;

    /**
     * @brief Bitboard containing all occupied squares.
     */
    uint64_t m_occupancy = 0ULL;    

    /**
     * @brief Castling rights stored as bit flags.
     *
     * Suggested layout:
     * bit 0 = white kingside
     * bit 1 = white queenside
     * bit 2 = black kingside
     * bit 3 = black queenside
     */
    CastlingRight m_castlingRights = CastlingRight::None;

    /**
     * @brief Current en passant target square.
     *
     * A value of -1 means no en passant square exists.
     */
    int32_t m_enPassantSquare = -1;

    /**
     * @brief Colour of the side to move.
     */
    PieceColour m_moverColour = PieceColour::None;

    /**
     * @brief Half-move clock used for the fifty-move rule.
     */
    int32_t m_halfMoveClock = 0;

    /**
     * @brief Full move number.
     *
     * Starts at 1 in normal chess notation.
     */
    int32_t m_fullMove = 1;

    /**
     * @brief Current board hash.
     */
    Hash_128 m_hash = {};



public:

    std::string toFEN() const;
    bool fromFEN(const std::string& fen);

public:

    /**
     * @brief Creates an empty BitBoard.
     *
     * All bitboards, occupancies, counters, castling rights,
     * en passant square, mover colour, and hash are initialized
     * using their default member values.
     */
    BitBoard() = default;

    /**
     * @brief Destroys the BitBoard.
     *
     * Default destructor is enough because this class does not
     * manually own heap memory or external resources.
     */
    ~BitBoard() = default;

    /**
     * @brief Creates a BitBoard from existing piece bitboards and state values.
     *
     * Copies the provided piece bitboards, stores the game-state metadata,
     * and rebuilds occupancy masks.
     *
     * @param bitboards Piece bitboards indexed by piece type and colour.
     * @param hasCastling Current castling rights.
     * @param enPassSq Current en passant target square, or Invalid_Square.
     * @param mover Colour of the side to move.
     * @param halfMove Current half-move clock.
     * @param fullMove Current full-move number.
     * @param hash Current board hash.
     */
    BitBoard(const uint64_t (&bitboards)[PieceTypeCount][PieceColourCount],
             CastlingRight hasCastling,
             int32_t enPassSq,
             PieceColour mover,
             int32_t halfMove,
             int32_t fullMove,
             Hash_128 hash) noexcept;

    /**
     * @brief Gets the colour of the side to move.
     *
     * @return Current mover colour.
     */
    PieceColour getMoversColour() const noexcept;

    /**
     * @brief Sets the colour of the side to move.
     *
     * Updates the board hash because side-to-move is part of
     * the Zobrist position identity.
     *
     * @param colour New mover colour.
     */
    void setMoversColour(PieceColour colour) noexcept;

    /**
     * @brief Rebuilds all occupancy bitboards from the piece bitboards.
     *
     * Recalculates white occupancy, black occupancy, and total occupancy.
     */
    void updateOccupancy() noexcept;

    /**
     * @brief Clears all occupancy bitboards.
     *
     * Does not modify the actual piece bitboards.
     */
    void clearOccupancy() noexcept;

    /**
     * @brief Gets the total board occupancy.
     *
     * @return Bitboard containing all occupied squares.
     */
    uint64_t getOccupancy() const noexcept;

    /**
     * @brief Sets the total occupancy bitboard manually.
     *
     * Usually this should be avoided unless restoring/debugging board state,
     * because occupancy is normally derived from the piece bitboards.
     *
     * @param occupancy New total occupancy bitboard.
     */
    void setOccupancy(uint64_t occupancy) noexcept;

    /**
     * @brief Gets the piece located on a square.
     *
     * @param square Board square index, normally 0..63.
     * @return Piece found on the square, or empty Piece if none exists.
     */
    Piece getPieceAt(int32_t square) const noexcept;

    /**
     * @brief Places a piece on a square.
     *
     * Clears any existing piece on the target square, places the new piece,
     * updates occupancy masks, and optionally updates the board hash.
     *
     * @param piece Piece to place.
     * @param square Target square index, normally 0..63.
     * @param hashUpdate Whether to recompute the board hash after setting.
     */
    void setPieceAt(const Piece& piece,
                    int32_t square,
                    bool hashUpdate = true) noexcept;

    /**
     * @brief Clears a square.
     *
     * Removes any piece from the given square, updates occupancy masks,
     * and optionally updates the board hash.
     *
     * @param square Square index to clear, normally 0..63.
     * @param hashUpdate Whether to recompute the board hash after clearing.
     */
    void clearSquareAt(int32_t square,
                       bool hashUpdate = true) noexcept;

    /**
     * @brief Removes and returns the piece from a square.
     *
     * If the square is empty or invalid, returns an empty Piece.
     *
     * @param square Square index to pop from.
     * @return Removed piece, or empty Piece.
     */
    Piece popPieceAt(int32_t square, bool updateHash = true) noexcept;

    /**
     * @brief Clears the entire board state.
     *
     * Resets the object to its default empty state.
     */
    void clear() noexcept;

    /**
     * @brief Initializes the board to the standard chess starting position.
     *
     * Sets all starting pieces, castling rights, side to move,
     * en passant state, counters, occupancy masks, and hash.
     */
    void init() noexcept;

    /**
     * @brief Gets current castling rights.
     *
     * @return Current castling-right flags.
     */
    CastlingRight getCastling() const noexcept;

    /**
     * @brief Sets current castling rights.
     *
     * Updates the board hash because castling rights are part of
     * the Zobrist position identity.
     *
     * @param castingRights New castling rights.
     */
    void setCastling(CastlingRight castingRights) noexcept;

    /**
     * @brief Gets a specific piece-type and colour bitboard.
     *
     * @param type Piece type.
     * @param colour Piece colour.
     * @return Bitboard for the requested piece type and colour,
     *         or 0ULL if the type/colour is invalid.
     */
    uint64_t        getBitBoard(PieceType type, PieceColour colour) const noexcept;
    uint64_t*       getBitBoardPtr(PieceType type, PieceColour colour) noexcept;
    uint64_t        occupiedSquares() const noexcept;

    const uint64_t (&getAllBitBoards() const noexcept)[PieceTypeCount][PieceColourCount];

    /**
     * @brief Sets a specific piece-type and colour bitboard.
     *
     * Rebuilds occupancy masks and updates the board hash.
     *
     * @param bitBoard New bitboard value.
     * @param type Piece type.
     * @param colour Piece colour.
     */
    void setBitBoard(uint64_t bitBoard,
                     PieceType type,
                     PieceColour colour) noexcept;

    /**
     * @brief Gets the current en passant target square.
     *
     * @return En passant square, or Invalid_Square if none exists.
     */
    int32_t getEnPassantSquare() const noexcept;

    /**
     * @brief Sets the current en passant target square.
     *
     * Accepts Invalid_Square for no en passant target.
     * Updates the board hash because en passant state is part of
     * the Zobrist position identity.
     *
     * @param square En passant target square, or Invalid_Square.
     */
    void setEnPassantSquare(int32_t square) noexcept;

    /**
     * @brief Gets the half-move clock.
     *
     * Used for the fifty-move rule.
     *
     * @return Current half-move clock.
     */
    int32_t getHalfMoveClock() const noexcept;

    /**
     * @brief Sets the half-move clock.
     *
     * Negative values are ignored.
     *
     * @param halfMoveClock New half-move clock.
     */
    void setHalfMoveClock(int32_t halfMoveClock) noexcept;

    /**
     * @brief Gets the current board hash.
     *
     * @return Current 128-bit board hash.
     */
    Hash_128 getHash() const noexcept;

    /**
     * @brief Sets the current board hash manually.
     *
     * Usually used when restoring a saved board state.
     *
     * @param hash New hash value.
     */
    void setHash(const Hash_128& hash) noexcept;

    /**
     * @brief Gets the full-move number.
     *
     * @return Current full-move number.
     */
    int32_t getFullMove() const noexcept;

    /**
     * @brief Sets the full-move number.
     *
     * Values below 1 are ignored.
     *
     * @param fullMove New full-move number.
     */
    void setFullMove(int32_t fullMove) noexcept;

    /**
     * @brief Computes the current board hash from scratch.
     *
     * Includes pieces, side to move, castling rights, and en passant file.
     * Does not include half-move clock or full-move number.
     *
     * @return Newly computed board hash.
     */
    Hash_128 computeHash() const noexcept;

    /**
     * @brief Recomputes and stores the current board hash.
     */
    void updateHash() noexcept;

    /**
     * @brief Gets the stored board hash by reference.
     *
     * @return Const reference to the current board hash.
     */
    const Hash_128& hash() const noexcept;

    /**
     * @brief Checks whether a specific castling right exists.
     *
     * @param right Castling right to test.
     * @return true if the right is currently available.
     * @return false otherwise.
     */
    bool hasCastlingRight(CastlingRight right) const noexcept;

    /**
     * @brief Adds a castling right.
     *
     * Updates the board hash.
     *
     * @param right Castling right to add.
     */
    void addCastlingRight(CastlingRight right) noexcept;

    /**
     * @brief Removes a castling right.
     *
     * Updates the board hash.
     *
     * @param right Castling right to remove.
     */
    void removeCastlingRight(CastlingRight right) noexcept;

    /**
     * @brief Moves a piece from one square to another.
     *
     * If the destination contains an enemy piece, the move is treated
     * as a capture. If the destination is empty, it is treated as a
     * normal move.
     *
     * This function does not validate chess legality such as check,
     * pins, castling rules, promotion rules, or en passant. It only
     * moves pieces at the bitboard level.
     *
     * @param from Source square index.
     * @param to Destination square index.
     * @return MoveType::Normal, MoveType::Capture, or MoveType::Invalid.
     */

    /**
     * Moves a piece from one square to another.
     */
    MoveType movePiece(int32_t from, int32_t to) noexcept;

    /**
     * Returns white piece occupancy.
     */
    uint64_t getWhiteOccupancy() const noexcept;

    /**
     * Sets white piece occupancy.
     */
    void setWhiteOccupancy(uint64_t whiteOccupancy) noexcept;

    /**
     * Returns black piece occupancy.
     */
    uint64_t getBlackOccupancy() const noexcept;

    /**
     * Sets black piece occupancy.
     */
    void setBlackOccupancy(uint64_t blackOccupancy) noexcept;

    /**
     * Returns all empty squares.
     */
    uint64_t getEmptySquares() const noexcept;

    /**
     * Switches the side to move.
     */
    void toggleMover() noexcept;
};





}

#endif // BITBOARD_H
