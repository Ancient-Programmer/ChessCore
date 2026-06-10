#ifndef ATTACKTABLES_H
#define ATTACKTABLES_H

#include <cstdint>



namespace ChessCore
{
namespace AttackTables
{
/**
 * Represents a ray direction on the board.
 */
struct RayDir
{
    int32_t rankStep;
    int32_t fileStep;
};

/**
 * Rook movement directions.
 */
enum RookDirCardinal : int32_t
{
    RookNorth = 0,
    RookSouth = 1,
    RookEast  = 2,
    RookWest  = 3
};

/**
 * Bishop movement directions.
 */
enum BishopDirCardinal : int32_t
{
    BishopNorthEast = 0,
    BishopNorthWest = 1,
    BishopSouthEast = 2,
    BishopSouthWest = 3
};

/**
 * Direction steps for rook rays.
 */
constexpr RayDir RookDirs[4] =
    {
        {  1,  0 }, // north
        { -1,  0 }, // south
        {  0,  1 }, // east
        {  0, -1 }  // west
};

/**
 * Direction steps for bishop rays.
 */
constexpr RayDir BishopDirs[4] =
    {
        {  1,  1 }, // north-east
        {  1, -1 }, // north-west
        { -1,  1 }, // south-east
        { -1, -1 }  // south-west
};

/**
 * Board square indexes.
 */
enum Squares
{
    a1 = 0, b1, c1, d1, e1, f1, g1, h1,
    a2 = 8, b2, c2, d2, e2, f2, g2, h2,
    a3 = 16, b3, c3, d3, e3, f3, g3, h3,
    a4 = 24, b4, c4, d4, e4, f4, g4, h4,
    a5 = 32, b5, c5, d5, e5, f5, g5, h5,
    a6 = 40, b6, c6, d6, e6, f6, g6, h6,
    a7 = 48, b7, c7, d7, e7, f7, g7, h7,
    a8 = 56, b8, c8, d8, e8, f8, g8, h8
};

/**
 * Precomputed king attack masks.
 */
extern uint64_t KingMask[64];

/**
 * Precomputed knight attack masks.
 */
extern uint64_t KnightMask[64];

/**
 * Precomputed white pawn attack masks.
 */
extern uint64_t WhitePawnMask[64];

/**
 * Precomputed black pawn attack masks.
 */
extern uint64_t BlackPawnMask[64];

/**
 * Precomputed directional rook ray masks.
 */
extern uint64_t RookRayMask[64][4];

/**
 * Precomputed directional bishop ray masks.
 */
extern uint64_t BishopRayMask[64][4];

}; // namespace AttackTables
}; // namespace ChessCore


#endif // ATTACKTABLES_H
