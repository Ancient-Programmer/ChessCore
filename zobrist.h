#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "chesstypes.h"



namespace ChessCore
{

namespace Zobrist
{

constexpr uint32_t BoardSquareCount = 64;
constexpr uint32_t CastlingStateCount = 16;
constexpr uint32_t FileCount = 8;

/**
 * @brief Random value for each piece, colour, and square.
 *
 * Indexed as:
 * PieceSquare[pieceType][pieceColour][square]
 */
extern uint64_t PieceSquare[PieceTypeCount][PieceColourCount][BoardSquareCount];

/**
 * @brief Random value XORed when black is to move.
 */
extern uint64_t SideToMove;

/**
 * @brief Random values for castling rights.
 *
 * Castling rights are stored as 4 bits:
 * bit 0 = white kingside
 * bit 1 = white queenside
 * bit 2 = black kingside
 * bit 3 = black queenside
 */
extern uint64_t Castling[CastlingStateCount];

/**
 * @brief Random values for en passant files.
 *
 * Only the file is hashed, not the exact square.
 */
extern uint64_t EnPassantFile[FileCount];

/**
 * @brief Initializes the Zobrist tables.
 *
 * Call once at program startup before hashing boards.
 */
void init(uint64_t seed = 0xC0FFEE123456789ULL) noexcept;

} // namespace Zobrist

} // namespace ChessCore

#endif // ZOBRIST_H
