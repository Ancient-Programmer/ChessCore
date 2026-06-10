#include <random>
#include "zobrist.h"



namespace ChessCore
{

namespace Zobrist
{

uint64_t PieceSquare[PieceTypeCount][PieceColourCount][BoardSquareCount] = {};
uint64_t SideToMove = 0ULL;
uint64_t Castling[CastlingStateCount] = {};
uint64_t EnPassantFile[FileCount] = {};

void init(uint64_t seed) noexcept
{
    std::mt19937_64 rng(seed);

    for(uint32_t piece = 0; piece < PieceTypeCount; ++piece)
    {
        for(uint32_t colour =0; colour < PieceColourCount; ++colour)
        {
            for(uint32_t square = 0; square < BoardSquareCount; ++square)
            {
                PieceSquare[piece][colour][square] = rng();
            }
        }
    }

    SideToMove = rng();

    for (uint32_t i = 0; i < CastlingStateCount; ++i)
    {
        Castling[i] = rng();
    }

    for (uint32_t file = 0; file < FileCount; ++file)
    {
        EnPassantFile[file] = rng();
    }
}

} // namespace Zobrist
} // namespace ChessCore
