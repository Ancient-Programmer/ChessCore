#ifndef BITUTILS_H
#define BITUTILS_H

#include <cstdint>

namespace ChessCore
{


/**
 * @class BitUtils
 * @brief Utility class for common chess bitboard operations.
 *
 * This class provides helper functions for converting between square indexes,
 * rank/file coordinates, and bitboard masks.
 *
 * The board is assumed to use 64 squares indexed from 0 to 63.
 *
 * Typical mapping:
 * - square 0  = rank 0, file 0
 * - square 7  = rank 0, file 7
 * - square 8  = rank 1, file 0
 * - square 63 = rank 7, file 7
 *
 * This class is static-only and cannot be instantiated.
 */
namespace BitUtils
{


/**
 * Bit mask for file A.
 *
 * Contains all squares from a1 to a8.
 */
constexpr uint64_t FileA = 0x0101010101010101ULL;

/**
 * Bit mask for file B.
 *
 * Contains all squares from b1 to b8.
 */
constexpr uint64_t FileB = 0x0202020202020202ULL;

/**
 * Bit mask for file C.
 *
 * Contains all squares from c1 to c8.
 */
constexpr uint64_t FileC = 0x0404040404040404ULL;

/**
 * Bit mask for file D.
 *
 * Contains all squares from d1 to d8.
 */
constexpr uint64_t FileD = 0x0808080808080808ULL;

/**
 * Bit mask for file E.
 *
 * Contains all squares from e1 to e8.
 */
constexpr uint64_t FileE = 0x1010101010101010ULL;

/**
 * Bit mask for file F.
 *
 * Contains all squares from f1 to f8.
 */
constexpr uint64_t FileF = 0x2020202020202020ULL;

/**
 * Bit mask for file G.
 *
 * Contains all squares from g1 to g8.
 */
constexpr uint64_t FileG = 0x4040404040404040ULL;

/**
 * Bit mask for file H.
 *
 * Contains all squares from h1 to h8.
 */
constexpr uint64_t FileH = 0x8080808080808080ULL;

/**
 * Bit mask for rank 1.
 *
 * Contains all squares from a1 to h1.
 */
constexpr uint64_t Rank1 = 0x00000000000000FFULL;

/**
 * Bit mask for rank 2.
 *
 * Contains all squares from a2 to h2.
 */
constexpr uint64_t Rank2 = 0x000000000000FF00ULL;

/**
 * Bit mask for rank 3.
 *
 * Contains all squares from a3 to h3.
 */
constexpr uint64_t Rank3 = 0x0000000000FF0000ULL;

/**
 * Bit mask for rank 4.
 *
 * Contains all squares from a4 to h4.
 */
constexpr uint64_t Rank4 = 0x00000000FF000000ULL;

/**
 * Bit mask for rank 5.
 *
 * Contains all squares from a5 to h5.
 */
constexpr uint64_t Rank5 = 0x000000FF00000000ULL;

/**
 * Bit mask for rank 6.
 *
 * Contains all squares from a6 to h6.
 */
constexpr uint64_t Rank6 = 0x0000FF0000000000ULL;

/**
 * Bit mask for rank 7.
 *
 * Contains all squares from a7 to h7.
 */
constexpr uint64_t Rank7 = 0x00FF000000000000ULL;

/**
 * Bit mask for rank 8.
 *
 * Contains all squares from a8 to h8.
 */
constexpr uint64_t Rank8 = 0xFF00000000000000ULL;

/**
 * Mask containing every square except file A.
 *
 * Useful for preventing horizontal wraparound when shifting bits.
 */
constexpr uint64_t NotFileA = ~FileA;

/**
 * Mask containing every square except file H.
 *
 * Useful for preventing horizontal wraparound when shifting bits.
 */
constexpr uint64_t NotFileH = ~FileH;

/**
 * Mask containing every square except files A and B.
 *
 * Useful for knight moves and other two-file shifts.
 */
constexpr uint64_t NotFileAB = ~(FileA | FileB);

/**
 * Mask containing every square except files G and H.
 *
 * Useful for knight moves and other two-file shifts.
 */
constexpr uint64_t NotFileGH = ~(FileG | FileH);

/**
 * Mask containing every square except rank 1.
 *
 * Useful for preventing downward rank overflow.
 */
constexpr uint64_t NotRank1 = ~Rank1;

/**
 * Mask containing every square except rank 8.
 *
 * Useful for preventing upward rank overflow.
 */
constexpr uint64_t NotRank8 = ~Rank8;

/**
 * @brief Invalid square value.
 *
 * Returned when a square, rank, or file operation fails.
 */
constexpr int32_t Invalid_Square = -1;

/**
 * @brief Minimum valid square index.
 */
constexpr int32_t Min_Square = 0;

/**
 * @brief Maximum valid square index.
 */
constexpr int32_t Max_Square = 63;

/**
 * @brief Number of squares per rank/file.
 */
constexpr int32_t Board_Dim = 8;

/**
 * @brief Total number of squares on the board.
 */
constexpr int32_t Board_Size = 64;

/**
 * @brief Minimum valid rank index.
 */
constexpr int32_t Min_Rank = 0;

/**
 * @brief Maximum valid rank index.
 */
constexpr int32_t Max_Rank = 7;

/**
 * @brief Minimum valid file index.
 */
constexpr int32_t Min_File = 0;

/**
 * @brief Maximum valid file index.
 */
constexpr int32_t Max_File = 7;



/**
 * @brief Converts a square index to a bitboard mask.
 *
 * Example:
 * @code
 * BitUtils::bitMask(0);  // 0x0000000000000001
 * BitUtils::bitMask(63); // 0x8000000000000000
 * @endcode
 *
 * @param square Square index in the range [0, 63].
 * @return A uint64_t bitboard with the square bit set.
 *         Returns 0 if the square is invalid.
 */
uint64_t bitMask(const int32_t) noexcept;

/**
 * @brief Converts a square index to its rank.
 *
 * This is equivalent to:
 * @code
 * square / 8
 * @endcode
 *
 * @param square Square index in the range [0, 63].
 * @return Rank index in the range [0, 7].
 *         Returns Invalid_Square if the square is invalid.
 */
int32_t toRank(const int32_t) noexcept;

/**
 * @brief Converts a square index to its file.
 *
 * This is equivalent to:
 * @code
 * square % 8
 * @endcode
 *
 * @param square Square index in the range [0, 63].
 * @return File index in the range [0, 7].
 *         Returns Invalid_Square if the square is invalid.
 */
int32_t toFile(const int32_t) noexcept;

/**
 * @brief Converts rank and file coordinates to a square index.
 *
 * Formula:
 * @code
 * square = rank * 8 + file
 * @endcode
 *
 * @param rank Rank index in the range [0, 7].
 * @param file File index in the range [0, 7].
 * @return Square index in the range [0, 63].
 *         Returns Invalid_Square if rank or file is invalid.
 */
int32_t toSquare(const int32_t rank, const int32_t file) noexcept;

/**
 * @brief Finds and removes the least significant set bit.
 *
 * This function returns the index of the least significant set bit in the
 * given bitboard, then clears that bit.
 *
 * Example:
 * @code
 * uint64_t bb = 0b101000;
 * int32_t sq = BitUtils::popLSB(&bb);
 * // sq == 3
 * // bb == 0b100000
 * @end
Piece       m_rookPiece;code
 *
 * @param bit Pointer to a bitboard.
 * @return Index of the least significant set bit.
 *         Returns Invalid_Square if the pointer is null or the bitboard is 0.
 */
int32_t popLSB(uint64_t*) noexcept;
int popCount(uint64_t bb) noexcept;

/**
 * @brief Gets the index of the least significant set bit.
 *
 * Unlike pop_LSB(), this function does not modify the bitboard.
 *
 * Example:
 * @code
 * uint64_t bb = 0b101000;
 * int32_t sq = BitUtils::indexLSB(bb);
 * // sq == 3
 * // bb is unchanged
 * @endcode
 *
 * @param bit Bitboard value.
 * @return Index of the least significant set bit.
 *         Returns Invalid_Square if the bitboard is 0.
 */
int32_t indexLSB(uint64_t) noexcept;

/**
 * @brief Gets the index of the most significant set bit.
 *
 *
 *
 * Example:
 * @code
 * uint64_t bb = 0b101000;
 * int32_t sq = BitUtils::indexMSB(bb);
 * // sq == 3
 * // bb is unchanged
 * @endcode
 *
 * @param  bit Bitboard value.
 * @return Index of the Most significant set bit.
 *         Returns Invalid_Square if the bitboard is 0.
 */
int32_t indexMSB(uint64_t bit) noexcept;


/**
 * @brief Checks whether a rank is valid.
 *
 * @param rank Rank index to test.
 * @return true if rank is in the range [0, 7], otherwise false.
 */
bool isValidRank(const int32_t) noexcept;

/**
 * @brief Checks whether a file is valid.
 *
 * @param file File index to test.
 * @return true if file is in the range [0, 7], otherwise false.
 */
bool isValidFile(const int32_t) noexcept;

/**
 * @brief Checks whether a square index is valid.
 *
 * @param square Square index to test.
 * @return true if square is in the range [0, 63], otherwise false.
 */
bool isValidSquare(const int32_t) noexcept;


    /**
 * Returns the intersection of two bitboards.
 *
 * Only squares that are set in both bitboards remain set.
 *
 * Equivalent to:
 * @code
 * a & b
 * @endcode
 *
 * @param a First bitboard.
 * @param b Second bitboard.
 * @return Bitboard containing only the common set squares.
 */
uint64_t intersection(uint64_t a, uint64_t b) noexcept;


    /**
 * Returns the union of two bitboards.
 *
 * All squares set in either bitboard will be set in the result.
 *
 * Equivalent to:
 * @code
 * a | b
 * @endcode
 *
 * @param a First bitboard.
 * @param b Second bitboard.
 * @return Bitboard containing all squares from both bitboards.
 */
uint64_t merge(uint64_t a, uint64_t b) noexcept;


    /**
 * Returns the difference between two bitboards.
 *
 * Keeps all squares from the first bitboard except those also set
 * in the second bitboard.
 *
 * Equivalent to:
 * @code
 * a & ~b
 * @endcode
 *
 * @param a Source bitboard.
 * @param b Bitboard containing squares to remove from @p a.
 * @return Bitboard containing squares from @p a that are not in @p b.
 */
uint64_t subtract(uint64_t a, uint64_t b) noexcept;


    /**
 * Returns the symmetric difference between two bitboards.
 *
 * Squares are set in the result only if they are set in exactly one
 * of the two input bitboards.
 *
 * Useful for detecting changed, toggled, moved, or differing squares.
 *
 * Equivalent to:
 * @code
 * a ^ b
 * @endcode
 *
 * @param a First bitboard.
 * @param b Second bitboard.
 * @return Bitboard containing squares that differ between @p a and @p b.
 */
uint64_t toggleDiff(uint64_t a, uint64_t b) noexcept;


    /**
 * Combines the white and black occupancy bitboards.
 *
 * The result contains every occupied square on the board, regardless
 * of piece colour.
 *
 * Equivalent to:
 * @code
 * whitePieces | blackPieces
 * @endcode
 *
 * @param whitePieces Bitboard containing all white pieces.
 * @param blackPieces Bitboard containing all black pieces.
 * @return Bitboard containing all occupied squares.
 */
uint64_t occupiedSquares(uint64_t whitePieces, uint64_t blackPieces) noexcept;


    /**
 * Returns all empty squares on the board.
 *
 * The result contains all valid board squares from @p fullboard that
 * are not occupied.
 *
 * Equivalent to:
 * @code
 * fullboard & ~occupied
 * @endcode
 *
 * @param fullboard Bitboard mask containing all valid board squares.
 * @param occupied Bitboard containing all occupied squares.
 * @return Bitboard containing all empty valid squares.
 */
uint64_t emptySquares(uint64_t fullboard, uint64_t occupied) noexcept;


    /**
 * Returns all capture target squares.
 *
 * The result contains only attacked squares that are occupied by enemy
 * pieces.
 *
 * Note:
 * The second parameter should normally be the enemy pieces bitboard,
 * not empty squares. The name @p emptyPieces would be misleading here,
 * because captures require occupied enemy squares. Classic naming trap,
 * nicely done by the species that also invented null pointers.
 *
 * Equivalent to:
 * @code
 * attacks & enemyPieces
 * @endcode
 *
 * @param attacks Bitboard containing all attacked squares.
 * @param enemyPieces Bitboard containing all enemy-occupied squares.
 * @return Bitboard containing valid capture targets.
 */
uint64_t capturedTargets(uint64_t attacks, uint64_t enemyPieces) noexcept;


    /**
 * Returns all quiet move target squares.
 *
 * Quiet targets are attacked or reachable squares that are empty.
 * These are non-capture moves.
 *
 * Equivalent to:
 * @code
 * attacks & ~occupied
 * @endcode
 *
 * @param attacks Bitboard containing all attacked or reachable squares.
 * @param occupied Bitboard containing all occupied squares.
 * @return Bitboard containing empty target squares.
 */
uint64_t quietTargets(uint64_t attacks, uint64_t occupied) noexcept;


    /**
 * Returns all movable target squares for a piece.
 *
 * The result contains all attacked or reachable squares except those
 * occupied by friendly pieces. This includes both empty squares and
 * enemy-occupied capture squares.
 *
 * Equivalent to:
 * @code
 * attacks & ~friendlyPieces
 * @endcode
 *
 * @param attacks Bitboard containing all attacked or reachable squares.
 * @param friendlyPieces Bitboard containing all friendly-occupied squares.
 * @return Bitboard containing all legal destination candidates before king-safety filtering.
 */
uint64_t movableTarget(uint64_t attacks, uint64_t friendlyPieces) noexcept;


    /**
 * Finds and removes the most significant set bit from a bitboard.
 *
 * The function returns the index of the highest set bit, then clears
 * that bit from the input bitboard.
 *
 * Example:
 * @code
 * uint64_t bb = 0b10110000;
 * int32_t idx = BitUtils::popMSB(&bb);
 *
 * // idx == 7
 * // bb  == 0b00110000
 * @endcode
 *
 * If @p bit is null or the bitboard is empty, Invalid_Square is returned.
 *
 * @param bit Pointer to the bitboard to modify.
 * @return Index of the removed most significant set bit, or Invalid_Square on failure.
 */
int32_t popMSB(uint64_t *bit) noexcept;

    /**
 * Returns true if the bitboard has no bits set.
 *
 * @param bitboard The bitboard to check.
 * @return True if bitboard is 0, false otherwise.
 */
bool isEmpty(uint64_t bitboard) noexcept;

    /**
 * Sets the bit at the given square index.
 *
 * Does nothing if bitboard is null or square is invalid.
 *
 * @param bitboard Pointer to the bitboard to modify.
 * @param square Square index in range [0, 63].
 */
void setBit(uint64_t* bitboard, int32_t square) noexcept;

    /**
 * Clears the bit at the given square index.
 *
 * Does nothing if bitboard is null or square is invalid.
 *
 * @param bitboard Pointer to the bitboard to modify.
 * @param square Square index in range [0, 63].
 */
void clearBit(uint64_t* bitboard, int32_t square) noexcept;

    /**
 * Returns true if the bit at the given square index is set.
 *
 * Returns false if the square is invalid.
 *
 * @param bitboard The bitboard to check.
 * @param square Square index in range [0, 63].
 * @return True if the bit is set, false otherwise.
 */
bool hasBit(uint64_t bitboard, int32_t square) noexcept;

    /**
 * Toggles the bit at the given square index.
 *
 * If the bit is 0, it becomes 1.
 * If the bit is 1, it becomes 0.
 * Does nothing if bitboard is null or square is invalid.
 *
 * @param bitboard Pointer to the bitboard to modify.
 * @param square Square index in range [0, 63].
 */
void toggleBit(uint64_t* bitboard, int32_t square) noexcept;

    /**
 * Clears the entire bitboard by setting it to 0.
 *
 * Does nothing if bitboard is null.
 *
 * @param bitboard Pointer to the bitboard to clear.
 */
void clearBitBoard(uint64_t* bitboard) noexcept;

    /**
 * Moves a bit from one square to another on the same bitboard.
 *
 * The source square must contain a bit and the destination square
 * must be empty.
 *
 * @param bitboard Pointer to the bitboard to modify.
 * @param from Source square index in range [0, 63].
 * @param to Destination square index in range [0, 63].
 * @return True if the bit was moved, false otherwise.
 */
bool moveBit(uint64_t* bitboard, int32_t from, int32_t to) noexcept;

    /**
 * Removes the bit at the given square index.
 *
 * @param bitboard Pointer to the bitboard to modify.
 * @param square Square index in range [0, 63].
 * @return True if a bit was removed, false otherwise.
 */
bool removeBit(uint64_t* bitboard, int32_t square) noexcept;

    /**
 * Adds a bit at the given square index.
 *
 * The target square must not already contain a bit.
 *
 * @param bitboard Pointer to the bitboard to modify.
 * @param square Square index in range [0, 63].
 * @return True if a bit was added, false otherwise.
 */
bool addBit(uint64_t* bitboard, int32_t square) noexcept;


// helpers
uint64_t bitsUpToInclusive(int32_t sq) noexcept;
uint64_t bitsFromInclusive(int32_t sq) noexcept;
uint64_t trimRay(uint64_t ray, uint64_t occupancy, bool increasingDirection = true) noexcept;

// Rank / File helpers / no comment needed

bool  isRank1(int32_t square) noexcept;
bool  isRank2(int32_t square) noexcept;
bool  isRank3(int32_t square) noexcept;
bool  isRank4(int32_t square) noexcept;
bool  isRank5(int32_t square) noexcept;
bool  isRank6(int32_t square) noexcept;
bool  isRank7(int32_t square) noexcept;
bool  isRank8(int32_t square) noexcept;

bool  isFileA(int32_t square) noexcept;
bool  isFileB(int32_t square) noexcept;
bool  isFileC(int32_t square) noexcept;
bool  isFileD(int32_t square) noexcept;
bool  isFileE(int32_t square) noexcept;
bool  isFileF(int32_t square) noexcept;
bool  isFileG(int32_t square) noexcept;
bool  isFileH(int32_t square) noexcept;


/**
 * Generates king attack squares from the given square.
 *
 * This returns pseudo-legal attack squares only.
 * Own-piece filtering and king-safety validation should be handled elsewhere.
 *
 * @param square Source square index in range [0, 63].
 * @return Bitboard containing all king attack squares, or 0 if the square is invalid.
 */
uint64_t kingAttacks(int32_t square) noexcept;

uint64_t rayAttacks(int32_t startRank,
                    int32_t startFile,
                    int32_t rankStep,
                    int32_t fileStep,
                    uint64_t blockers = 0ULL) noexcept;
/**
 * Generates rook attack squares from the given square.
 *
 * Sliding stops at the first blocker in each orthogonal direction.
 * The blocker square itself is included as an attack square.
 * Own-piece filtering should be handled after this function with:
 * attacks & ~ownPieces
 *
 * @param square Source square index in range [0, 63].
 * @param blockers Bitboard containing all occupied squares.
 * @return Bitboard containing all rook attack squares, or 0 if the square is invalid.
 */
uint64_t rookAttacks(int32_t square, uint64_t blockers = 0ULL) noexcept;

/**
 * Generates bishop attack squares from the given square.
 *
 * Sliding stops at the first blocker in each diagonal direction.
 * The blocker square itself is included as an attack square.
 * Own-piece filtering should be handled after this function with:
 * attacks & ~ownPieces
 *
 * @param square Source square index in range [0, 63].
 * @param blockers Bitboard containing all occupied squares.
 * @return Bitboard containing all bishop attack squares, or 0 if the square is invalid.
 */
uint64_t bishopAttacks(int32_t square, uint64_t blockers = 0ULL) noexcept;

/**
 * Generates queen attack squares from the given square.
 *
 * Equivalent to rook attacks combined with bishop attacks.
 * Sliding stops at the first blocker in each direction.
 * Own-piece filtering should be handled after this function with:
 * attacks & ~ownPieces
 *
 * @param square Source square index in range [0, 63].
 * @param blockers Bitboard containing all occupied squares.
 * @return Bitboard containing all queen attack squares, or 0 if the square is invalid.
 */
uint64_t queenAttacks(int32_t square, uint64_t blockers = 0ULL) noexcept;

/**
 * Generates knight attack squares from the given square.
 *
 * This returns pseudo-legal attack squares only.
 * Own-piece filtering should be handled elsewhere with:
 * attacks & ~ownPieces
 *
 * @param square Source square index in range [0, 63].
 * @return Bitboard containing all knight attack squares, or 0 if the square is invalid.
 */
uint64_t knightAttacks(int32_t square) noexcept;

/**
 * Generates pawn diagonal attack squares from the given square.
 *
 * This does not include forward pawn moves.
 * This does not check whether enemy pieces are present.
 * For white pawns, attacks go toward higher ranks.
 * For black pawns, attacks go toward lower ranks.
 *
 * @param square Source square index in range [0, 63].
 * @param isWhite True for white pawn attacks, false for black pawn attacks.
 * @return Bitboard containing pawn attack squares, or 0 if the square is invalid.
 */
uint64_t pawnAttacks(int32_t square, bool isWhite = true) noexcept;

/**
 * Generates a single forward pawn push from the given square.
 *
 * The target square must be empty.
 * This function does not generate captures.
 *
 * @param square Source square index in range [0, 63].
 * @param isWhite True for a white pawn, false for a black pawn.
 * @param emptySquares Bitboard containing all empty squares.
 * @return Bitboard containing the single push target square, or 0 if unavailable.
 */
uint64_t pawnSinglePush(int32_t square,
                        bool isWhite = true,
                        uint64_t emptySquares = 0ULL) noexcept;

/**
 * Generates a double forward pawn push from the starting rank.
 *
 * The pawn must be on its starting rank.
 * Both the single-push square and double-push square must be empty.
 * This function does not generate captures.
 *
 * @param square Source square index in range [0, 63].
 * @param isWhite True for a white pawn, false for a black pawn.
 * @param emptySquares Bitboard containing all empty squares.
 * @return Bitboard containing the double push target square, or 0 if unavailable.
 */
uint64_t pawnDoublePush(int32_t square,
                        bool isWhite = true,
                        uint64_t emptySquares = 0ULL) noexcept;

/**
 * Generates an en passant capture target from the given pawn square.
 *
 * The en passant square is the square the pawn moves to, not the square
 * of the captured pawn. The captured pawn should be removed later when
 * applying the move. Yes, pawns are ridiculous little rule gremlins.
 *
 * @param square Source square index in range [0, 63].
 * @param isWhite True for a white pawn, false for a black pawn.
 * @param enPassantSquare Current en passant target square, or Invalid_Square.
 * @return Bitboard containing the en passant target square, or 0 if unavailable.
 */
uint64_t pawnEnPassant(int32_t square,
                       bool isWhite = true,
                       int32_t enPassantSquare = Invalid_Square) noexcept;

/**
 * Generates normal pawn capture targets from the given square.
 *
 * Captures are calculated by intersecting pawn diagonal attacks with
 * enemy pieces. This function does not include en passant unless you
 * explicitly handle it elsewhere.
 *
 * @param square Source square index in range [0, 63].
 * @param isWhite True for a white pawn, false for a black pawn.
 * @param enemyPieces Bitboard containing all enemy pieces.
 * @return Bitboard containing normal pawn capture target squares.
 */
uint64_t pawnCaptures(int32_t square,
                      bool isWhite = true,
                      uint64_t enemyPieces = 0ULL) noexcept;

/**
 * Generates pseudo-legal pawn moves from the given square.
 *
 * Includes:
 * - single forward push
 * - double forward push
 * - normal captures
 * - en passant capture
 *
 * This function does not check king safety.
 *
 * @param square Source square index in range [0, 63].
 * @param isWhite True for a white pawn, false for a black pawn.
 * @param emptySquares Bitboard containing all empty squares.
 * @param enemyPieces Bitboard containing all enemy pieces.
 * @param enPassantSquare Current en passant target square, or Invalid_Square.
 * @return Bitboard containing all pseudo-legal pawn move target squares.
 */
uint64_t pawnMoves(int32_t square,
                   bool isWhite = true,
                   uint64_t emptySquares = 0ULL,
                   uint64_t enemyPieces = 0ULL,
                   int32_t enPassantSquare = Invalid_Square) noexcept;

} // namespace Bitutils


} // namespace ChessCore
#endif // BITUTILS_H
