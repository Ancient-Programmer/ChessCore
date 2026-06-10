#ifndef UCIUTILS_H
#define UCIUTILS_H

#include "chesstypes.h"
#include <string>


namespace ChessCore
{

/**
 * Utility class for UCI and FEN conversions.
 *
 * Provides helpers for converting board state to/from FEN,
 * converting moves to/from UCI notation, and converting squares
 * to/from UCI square strings.
 */
class UciUtils
{
public:
    /**
     * Converts raw bitboards and board state into a FEN string.
     */
    static std::string bitboardsToFen(const uint64_t (&bitboards)[PieceTypeCount][PieceColourCount],
                                      PieceColour moverColour,
                                      CastlingRight castlingRights,
                                      int32_t enPassantSquare,
                                      int32_t halfMoveClock,
                                      int32_t fullMove);

    /**
     * Parses a FEN string into raw bitboards and board state.
     */
    static bool fenToBitboards(const std::string& fen,
                               uint64_t (&bitboards)[PieceTypeCount][PieceColourCount],
                               PieceColour& moverColour,
                               CastlingRight& castlingRights,
                               int32_t& enPassantSquare,
                               int32_t& halfMoveClock,
                               int32_t& fullMove);

    /**
     * Converts a move into UCI notation.
     *
     * Example: e2e4, e7e8q.
     */
    static std::string moveToUci(const Move& move);

    /**
     * Converts UCI notation into a move.
     *
     * Example: e2e4, e7e8q.
     */
    static Move uciToMove(const std::string& uci);

    /**
     * Converts a square index into UCI square notation.
     *
     * Example: 12 -> e2.
     */
    static std::string squareToUci(int32_t square);

    /**
     * Converts UCI file/rank characters into a square index.
     *
     * Example: 'e', '2' -> 12.
     */
    static int32_t uciToSquare(char fileChar, char rankChar);

};

} // namespace ChessCore


#endif // UCIUTILS_H
