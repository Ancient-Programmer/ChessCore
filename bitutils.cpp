#include "bitutils.h"

#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L
#include <bit>
#endif

#if defined(QT_VERSION)
#include <QtGlobal>
#include <QtAlgorithms>
#endif

#if defined(_MSC_VER)
#include <intrin.h>
#endif


namespace ChessCore
{


namespace BitUtils
{

uint64_t bitMask(const int32_t square) noexcept
{
    return isValidSquare(square)
    ? (uint64_t{1} << square)
    : uint64_t{0};
}


int32_t toRank(const int32_t square) noexcept
{
    return isValidSquare(square)
    ? (square >> 3)
    : Invalid_Square;
}

int32_t toFile(const int32_t square) noexcept
{
    return isValidSquare(square)
    ? (square & Max_File)
    : Invalid_Square;
}

int32_t toSquare(const int32_t rank, const int32_t file) noexcept
{
    return isValidRank(rank) && isValidFile(file)
    ? (rank * Board_Dim + file)
    : Invalid_Square;
}

bool isValidRank(const int32_t rank) noexcept
{
    return rank >= Min_Rank && rank <= Max_Rank;
}

bool isValidFile(const int32_t file) noexcept
{
    return file >= Min_File && file <= Max_File;
}

bool isValidSquare(const int32_t square) noexcept
{
    return square >= Min_Square && square <= Max_Square;
}

uint64_t intersection(uint64_t a, uint64_t b) noexcept
{
    return a & b;
}

uint64_t merge(uint64_t a, uint64_t b) noexcept
{
    return a | b;
}

uint64_t subtract(uint64_t a, uint64_t b) noexcept
{
    return a & ~b;
}

uint64_t toggleDiff(uint64_t a, uint64_t b) noexcept
{
    return a ^ b;
}

uint64_t occupiedSquares(uint64_t whitePieces, uint64_t blackPieces) noexcept
{
    return whitePieces | blackPieces;
}

uint64_t emptySquares(uint64_t fullboard, uint64_t occupied) noexcept
{
    return fullboard & ~occupied;
}

uint64_t capturedTargets(uint64_t attacks, uint64_t emptyPieces) noexcept
{
    return attacks & emptyPieces;
}

uint64_t quietTargets(uint64_t attacks, uint64_t occupied) noexcept
{
    return attacks & ~occupied;
}

uint64_t movableTarget(uint64_t attacks, uint64_t friendlyPieces) noexcept
{
    return attacks & ~friendlyPieces;
}


int32_t popLSB(uint64_t* bit) noexcept
{
    if (bit == nullptr || *bit == 0)
        return Invalid_Square;

    int32_t idx = Invalid_Square;

#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L

    idx = static_cast<int32_t>(std::countr_zero(*bit));

#elif defined(QT_VERSION)

    idx = static_cast<int32_t>(qCountTrailingZeroBits(*bit));

#elif defined(_MSC_VER)

    unsigned long index = 0;
    _BitScanForward64(&index, *bit);
    idx = static_cast<int32_t>(index);

#elif defined(__GNUC__) || defined(__clang__)

    idx = static_cast<int32_t>(__builtin_ctzll(*bit));

#else

    uint64_t bb = *bit;
    idx = 0;

    while ((bb & 1ULL) == 0)
    {
        bb >>= 1;
        ++idx;
    }

#endif
    *bit &= (*bit - 1);
    return idx;
}


int32_t popMSB(uint64_t* bit) noexcept
{
    if (bit == nullptr || *bit == 0ULL)
        return Invalid_Square;

    int32_t idx = Invalid_Square;

#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L

    idx = static_cast<int32_t>(Max_Square - std::countl_zero(*bit));

#elif defined(QT_VERSION)

    idx = static_cast<int32_t>(Max_Square - qCountLeadingZeroBits(*bit));

#elif defined(_MSC_VER)

    unsigned long index = 0;
    _BitScanReverse64(&index, *bit);
    idx = static_cast<int32_t>(index);

#elif defined(__GNUC__) || defined(__clang__)

    idx = static_cast<int32_t>(63 - __builtin_clzll(*bit));

#else

    uint64_t bb = *bit;
    idx = 0;

    while (bb >>= 1ULL)
    {
        ++idx;
    }

#endif

    // Clear the MSB we just found
    *bit &= ~bitMask(idx);

    return idx;
}

bool isEmpty(uint64_t bitboard) noexcept
{
    return bitboard == 0ULL;
}

void setBit(uint64_t *bitboard, int32_t square) noexcept
{

    if (!bitboard || !isValidSquare(square))
        return;

    *bitboard |= bitMask(square);
}

void clearBit(uint64_t *bitboard, int32_t square) noexcept
{
    if (!bitboard || !isValidSquare(square))
        return;

    *bitboard &= ~bitMask(square);
}

bool hasBit(uint64_t bitboard, int32_t square) noexcept
{
    if (!isValidSquare(square))
        return false;

    return ((bitboard & bitMask(square)) != 0ULL);
}

void toggleBit(uint64_t *bitboard, int32_t square) noexcept
{
    if (!bitboard || !isValidSquare(square))
        return;

    *bitboard ^= bitMask(square);
}

void clearBitBoard(uint64_t *bitboard) noexcept
{
    if (!bitboard)
        return;

    *bitboard = 0ULL;
}

bool moveBit(uint64_t *bitboard, int32_t from, int32_t to) noexcept
{
    if (!bitboard || !isValidSquare(from) || !isValidSquare(to))
        return false;

    if (from == to)
        return false;

    uint64_t fromMask = bitMask(from);
    uint64_t toMask   = bitMask(to);

    // Source square must contain a bit
    if ((*bitboard & fromMask) == 0ULL)
        return false;

    // Destination square must be empty
    if ((*bitboard & toMask) != 0ULL)
        return false;

    *bitboard &= ~fromMask;
    *bitboard |= toMask;

    return true;
}

bool removeBit(uint64_t *bitboard, int32_t square) noexcept
{
    if (!bitboard || !isValidSquare(square))
        return false;

    uint64_t mask = bitMask(square);

    if ((*bitboard & mask) == 0ULL)
        return false;

    *bitboard &= ~mask;
    return true;
}

bool addBit(uint64_t *bitboard, int32_t square) noexcept
{
    if (!bitboard || !isValidSquare(square))
        return false;

    uint64_t mask = bitMask(square);

    if ((*bitboard & mask) != 0ULL)
        return false;

    *bitboard |= mask;
    return true;
}

int32_t indexLSB(uint64_t bit) noexcept
{
    if (bit == 0)
        return Invalid_Square;

#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L

    return static_cast<int32_t>(std::countr_zero(bit));

#elif defined(QT_VERSION)

    return static_cast<int32_t>(qCountTrailingZeroBits(bit));

#elif defined(_MSC_VER)

    unsigned long index = 0;
    _BitScanForward64(&index, bit);
    return static_cast<int32_t>(index);

#elif defined(__GNUC__) || defined(__clang__)

    return static_cast<int32_t>(__builtin_ctzll(bit));

#else

    int32_t idx = 0;
    while ((bit & 1ULL) == 0)
    {
        bit >>= 1;
        ++idx;
    }

    return idx;

#endif
}


int popCount(uint64_t bb) noexcept
{
#if defined(QT_VERSION)
    return static_cast<int>(qPopulationCount(bb));

#elif defined(__GNUC__) || defined(__clang__)
    return __builtin_popcountll(bb);

#elif defined(_MSC_VER)
    return static_cast<int>(__popcnt64(bb));

#else
    int count = 0;

    while (bb)
    {
        bb &= bb - 1ULL;
        ++count;
    }

    return count;
#endif
}

int32_t indexMSB(uint64_t bit) noexcept
{
    if (bit == 0ULL)
        return Invalid_Square;

#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L

    return static_cast<int32_t>(Max_Square - std::countl_zero(bit));

#elif defined(QT_VERSION)

    return static_cast<int32_t>(Max_Square - qCountLeadingZeroBits(bit));

#elif defined(_MSC_VER)

    unsigned long index = 0;
    _BitScanReverse64(&index, bit);
    return static_cast<int32_t>(index);

#elif defined(__GNUC__) || defined(__clang__)

    return static_cast<int32_t>(63 - __builtin_clzll(bit));

#else

    int32_t index = 0;

    while (bit >>= 1ULL)
    {
        ++index;
    }

    return index;

#endif
}

bool  isRank1(int32_t square) noexcept { return hasBit(Rank1, square); }
bool  isRank2(int32_t square) noexcept { return hasBit(Rank2, square); }
bool  isRank3(int32_t square) noexcept { return hasBit(Rank3, square); }
bool  isRank4(int32_t square) noexcept { return hasBit(Rank4, square); }
bool  isRank5(int32_t square) noexcept { return hasBit(Rank5, square); }
bool  isRank6(int32_t square) noexcept { return hasBit(Rank6, square); }
bool  isRank7(int32_t square) noexcept { return hasBit(Rank7, square); }
bool  isRank8(int32_t square) noexcept { return hasBit(Rank8, square); }

bool  isFileA(int32_t square) noexcept { return hasBit(FileA, square); }
bool  isFileB(int32_t square) noexcept { return hasBit(FileB, square); }
bool  isFileC(int32_t square) noexcept { return hasBit(FileC, square); }
bool  isFileD(int32_t square) noexcept { return hasBit(FileD, square); }
bool  isFileE(int32_t square) noexcept { return hasBit(FileE, square); }
bool  isFileF(int32_t square) noexcept { return hasBit(FileF, square); }
bool  isFileG(int32_t square) noexcept { return hasBit(FileG, square); }
bool  isFileH(int32_t square) noexcept { return hasBit(FileH, square); }

uint64_t kingAttacks(int32_t square) noexcept
{
    const uint64_t king = bitMask(square);

    if (king == 0ULL)
        return 0ULL;

    uint64_t moves = 0ULL;

    // Vertical moves
    moves |= king << Board_Dim;              // north
    moves |= king >> Board_Dim;              // south

    // Horizontal moves
    moves |= (king << 1) & NotFileA; // east
    moves |= (king >> 1) & NotFileH; // west

    // Diagonal moves
    moves |= (king << (Board_Dim + 1)) & NotFileA; // north-east
    moves |= (king << (Board_Dim - 1)) & NotFileH; // north-west
    moves |= (king >> (Board_Dim - 1)) & NotFileA; // south-east
    moves |= (king >> (Board_Dim + 1)) & NotFileH; // south-west

    // Remove own occupied squares
    return moves;
}


uint64_t trimRay(uint64_t ray, uint64_t occupancy, bool increasingDirection) noexcept
{
    const uint64_t blockersOnRay = ray & occupancy;

    if(blockersOnRay == 0ULL)
    {
        return ray;
    }

    const int32_t blockerSq = increasingDirection?
                              BitUtils::indexLSB(blockersOnRay):
                              BitUtils::indexMSB(blockersOnRay);

    if(increasingDirection)
    {
        // Keep ray squares up to and including the first blocker.
        return ray & bitsUpToInclusive(blockerSq);
    }

    // Keep ray squares down to and including the first blocker.
    return ray & bitsFromInclusive(blockerSq);
}

uint64_t rayAttacks(int32_t startRank,
                    int32_t startFile,
                    int32_t rankStep,
                    int32_t fileStep,
                    uint64_t blockers) noexcept
{
    uint64_t moves = 0ULL;

    int32_t rank = startRank + rankStep;
    int32_t file = startFile + fileStep;

    while (rank >= Min_Square && rank < Board_Dim &&
           file >= Min_Square && file < Board_Dim)
    {
        const int32_t sq = rank * Board_Dim + file;
        const uint64_t mask = bitMask(sq);

        moves |= mask;

        if (blockers & mask)
            break;

        rank += rankStep;
        file += fileStep;
    }

    return moves;
}

uint64_t rookAttacks(int32_t square, uint64_t blockers) noexcept
{
    if (!isValidSquare(square))
        return 0ULL;

    const int32_t rank = square / Board_Dim;
    const int32_t file = square % Board_Dim;

    uint64_t moves = 0ULL;

    moves |= rayAttacks(rank, file,  1,  0, blockers); // north
    moves |= rayAttacks(rank, file, -1,  0, blockers); // south
    moves |= rayAttacks(rank, file,  0,  1, blockers); // east
    moves |= rayAttacks(rank, file,  0, -1, blockers); // west

    return moves;
}


uint64_t bitsUpToInclusive(int32_t sq) noexcept
{
    if (sq < Min_Square)
        return 0ULL;

    if (sq >= Max_Square)
        return UINT64_MAX;

    return (uint64_t{1} << (sq + 1)) - 1ULL;
}

uint64_t bitsFromInclusive(int32_t sq) noexcept
{
    if (sq <= Min_Square)
        return UINT64_MAX;

    if (sq > Max_Square)
        return 0ULL;

    return ~((uint64_t{1} << sq) - 1ULL);
}

uint64_t bishopAttacks(int32_t square, uint64_t blockers) noexcept
{
    if (!isValidSquare(square))
        return 0ULL;

    const int32_t rank = square / Board_Dim;
    const int32_t file = square % Board_Dim;

    uint64_t moves = 0ULL;

    moves |= rayAttacks(rank, file,  1,  1, blockers); // north-east
    moves |= rayAttacks(rank, file,  1, -1, blockers); // north-west
    moves |= rayAttacks(rank, file, -1,  1, blockers); // south-east
    moves |= rayAttacks(rank, file, -1, -1, blockers); // south-west

    return moves;
}

uint64_t queenAttacks(int32_t square) noexcept
{
    return rookAttacks(square) |
           bishopAttacks(square);
}


uint64_t knightAttacks(int32_t square) noexcept
{
    const uint64_t knight = bitMask(square);

    if (knight == 0ULL)
        return 0ULL;

    uint64_t moves = 0ULL;

    // 2 north, 1 east/west
    moves |= (knight << ((Board_Dim * 2) +1)) & NotFileA;   // north-north-east
    moves |= (knight << ((Board_Dim * 2) -1)) & NotFileH;   // north-north-west

    // 1 north, 2 east/west
    moves |= (knight << (Board_Dim + 2)) & NotFileAB;  // north-east-east
    moves |= (knight << (Board_Dim - 2))  & NotFileGH;  // north-west-west

    // 2 south, 1 east/west
    moves |= (knight >> ((Board_Dim * 2) - 1)) & NotFileA;   // south-south-east
    moves |= (knight >> ((Board_Dim * 2) + 1)) & NotFileH;   // south-south-west

    // 1 south, 2 east/west
    moves |= (knight >>(Board_Dim - 2))  & NotFileAB;  // south-east-east
    moves |= (knight >>(Board_Dim + 2)) & NotFileGH;  // south-west-west



    // Remove own occupied squares
    return moves;
}


uint64_t pawnAttacks(int32_t square, bool isWhite) noexcept
{
    const uint64_t pawn = bitMask(square);

    if (pawn == 0ULL)
        return 0ULL;

    uint64_t attacks = 0ULL;

    if (isWhite)
    {
        attacks |= (pawn << (Board_Dim + 1)) & NotFileA; // north-east
        attacks |= (pawn << (Board_Dim - 1)) & NotFileH; // north-west
    }
    else
    {
        attacks |= (pawn >> (Board_Dim - 1)) & NotFileA; // south-east
        attacks |= (pawn >> (Board_Dim + 1)) & NotFileH; // south-west
    }

    return attacks;
}


uint64_t pawnSinglePush(int32_t square, bool isWhite, uint64_t emptySquares) noexcept
{
    const uint64_t pawn = bitMask(square);

    if(pawn == 0ULL)
    {
        return 0ULL;
    }

    if(isWhite)
    {
        return (pawn << Board_Dim) & emptySquares;
    }

    return (pawn >> Board_Dim) & emptySquares;
};


uint64_t pawnDoublePush(int32_t square, bool isWhite, uint64_t emptySquares) noexcept
{
    const uint64_t pawn = bitMask(square);

    if (pawn == 0ULL)
        return 0ULL;

    const uint64_t startRank = isWhite ? Rank2 : Rank7;

    if ((pawn & startRank) == 0ULL)
        return 0ULL;

    if (isWhite)
    {
        const uint64_t oneStep = (pawn << Board_Dim) & emptySquares;

        if (oneStep == 0ULL)
            return 0ULL;

        return (oneStep << Board_Dim) & emptySquares;
    }
    else
    {
        const uint64_t oneStep = (pawn >> Board_Dim) & emptySquares;

        if (oneStep == 0ULL)
            return 0ULL;

        return (oneStep >> Board_Dim) & emptySquares;
    }
}


uint64_t pawnEnPassant(int32_t square, bool isWhite, int32_t enPassantSquare) noexcept
{
    if (!isValidSquare(enPassantSquare))
        return 0ULL;

    const uint64_t attacks = pawnAttacks(square, isWhite);
    const uint64_t epMask = bitMask(enPassantSquare);

    return attacks & epMask;
}

uint64_t pawnCaptures(int32_t square, bool isWhite, uint64_t enemyPieces) noexcept
{
    return pawnAttacks(square, isWhite) & enemyPieces;
}

uint64_t pawnMoves(int32_t square,
                   bool isWhite,
                   uint64_t emptySquares,
                   uint64_t enemyPieces,
                   int32_t enPassantSquare) noexcept
{
    if (!isValidSquare(square))
        return 0ULL;

    uint64_t moves = 0ULL;

    moves |= pawnSinglePush(square, isWhite, emptySquares);
    moves |= pawnDoublePush(square, isWhite, emptySquares);
    moves |= pawnCaptures(square, isWhite, enemyPieces);
    moves |= pawnEnPassant(square, isWhite, enPassantSquare);

    return moves;
}



} //  end of Bitutils
} // end of ChessCore
