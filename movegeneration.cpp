#include <cstdlib>
#include <cassert>



#include "movegeneration.h"
#include "bitutils.h"
#include "attacktables.h"
#include "uciutils.h"
#include "ChessDebug.h"





namespace ChessCore
{

namespace MoveGeneration
{


void initAttackTables() noexcept
{
    // king attacks
    for(int32_t sq = BitUtils::Min_Square; sq <= BitUtils::Max_Square; ++sq)
    {

        const int32_t rank = sq / BitUtils::Board_Dim;
        const int32_t file = sq % BitUtils::Board_Dim;

        AttackTables::KingMask[sq] = BitUtils::kingAttacks(sq);
        AttackTables::KnightMask[sq] = BitUtils::knightAttacks(sq);
        AttackTables::WhitePawnMask[sq] = BitUtils::pawnAttacks(sq, true);
        AttackTables::BlackPawnMask[sq] = BitUtils::pawnAttacks(sq, false);

        for(int32_t dir=0; dir < 4; ++dir)
        {
            AttackTables::RookRayMask[sq][dir] = BitUtils::rayAttacks(
                rank,
                file,
                AttackTables::RookDirs[dir].rankStep,
                AttackTables::RookDirs[dir].fileStep
                );

            AttackTables::BishopRayMask[sq][dir] = BitUtils::rayAttacks(
                rank,
                file,
                AttackTables::BishopDirs[dir].rankStep,
                AttackTables::BishopDirs[dir].fileStep
                );
        }
    }
}





uint64_t rookAttacksFromMask(int32_t square, uint64_t occupancy) noexcept
{
    if(!BitUtils::isValidSquare(square))
    {
        return 0ULL;
    }

    uint64_t attacks = 0ULL;

    attacks |= BitUtils::trimRay(
        AttackTables::RookRayMask[square][AttackTables::RookNorth],
        occupancy,
        true);

    attacks |= BitUtils::trimRay(
        AttackTables::RookRayMask[square][AttackTables::RookSouth],
        occupancy,
        false);

    attacks |= BitUtils::trimRay(
        AttackTables::RookRayMask[square][AttackTables::RookEast],
        occupancy,
        true);

    attacks |= BitUtils::trimRay(
        AttackTables::RookRayMask[square][AttackTables::RookWest],
        occupancy,
        false);

    return attacks;
}

uint64_t bishopAttacksFromMask(int32_t square, uint64_t occupancy) noexcept
{
    if(!BitUtils::isValidSquare(square))
    {
        return 0ULL;
    }

    uint64_t attacks = 0ULL;

    attacks |= BitUtils::trimRay(
        AttackTables::BishopRayMask[square][AttackTables::BishopNorthEast],
        occupancy,
        true);

    attacks |= BitUtils::trimRay(
        AttackTables::BishopRayMask[square][AttackTables::BishopNorthWest],
        occupancy,
        true);

    attacks |= BitUtils::trimRay(
        AttackTables::BishopRayMask[square][AttackTables::BishopSouthEast],
        occupancy,
        false);

    attacks |= BitUtils::trimRay(
        AttackTables::BishopRayMask[square][AttackTables::BishopSouthWest],
        occupancy,
        false);

    return attacks;
}

uint64_t queenAttacksFromMask(int32_t square, uint64_t occupancy) noexcept
{
    return rookAttacksFromMask(square, occupancy) | bishopAttacksFromMask(square, occupancy);
}


/* older version loop based
 *
bool isSquareAttacked(const BitBoard &Board, int32_t square, PieceColour colour) noexcept
{
    if (!BitUtils::isValidSquare(square))
        return false;

    const PieceColour attackersColour =
        (colour == PieceColour::White)
            ? PieceColour::Black
            : PieceColour::White;

    const uint64_t attackersPawns = Board.getBitBoard(PieceType::Pawn, attackersColour);
    const uint64_t attackersKnights = Board.getBitBoard(PieceType::Knight, attackersColour);
    const uint64_t attackersBishops = Board.getBitBoard(PieceType::Bishop, attackersColour);
    const uint64_t attackersRooks = Board.getBitBoard(PieceType::Rook, attackersColour);
    const uint64_t attackersQueens = Board.getBitBoard(PieceType::Queen, attackersColour);
    const uint64_t attackersKing = Board.getBitBoard(PieceType::King, attackersColour);
    const uint64_t occupancy = Board.getOccupancy();

    if (attackersColour == PieceColour::White)
    {
        if (AttackTables::BlackPawnMask[square] & attackersPawns)
            return true;
    }
    else
    {
        if (AttackTables::WhitePawnMask[square] & attackersPawns)
            return true;
    }

    // Knights are symmetric.
    if (AttackTables::KnightMask[square] & attackersKnights)
        return true;

    // Kings are symmetric.
    if (AttackTables::KingMask[square] & attackersKing)
        return true;


    // Rook-like attacks: rook or queen.
    if (BitUtils::rookAttacks(square, occupancy) &
        (attackersRooks | attackersQueens))
    {
        return true;
    }

    // Bishop-like attacks: bishop or queen.
    if (BitUtils::bishopAttacks(square, occupancy) &
        (attackersBishops | attackersQueens))
    {
        return true;
    }
    return false;
}

*/

bool isSquareAttacked(const BitBoard& board,
                      int32_t square,
                      PieceColour colour) noexcept
{

#if CHESS_DEBUG_BUILD
    CHESS_ASSERT(BitUtils::isValidSquare(square));
#endif

    const PieceColour attackersColour =
        (colour == PieceColour::White)
            ? PieceColour::Black
            : PieceColour::White;

    const uint64_t attackersPawns =
        board.getBitBoard(PieceType::Pawn, attackersColour);

    const uint64_t attackersKnights =
        board.getBitBoard(PieceType::Knight, attackersColour);

    const uint64_t attackersBishops =
        board.getBitBoard(PieceType::Bishop, attackersColour);

    const uint64_t attackersRooks =
        board.getBitBoard(PieceType::Rook, attackersColour);

    const uint64_t attackersQueens =
        board.getBitBoard(PieceType::Queen, attackersColour);

    const uint64_t attackersKing =
        board.getBitBoard(PieceType::King, attackersColour);

    const uint64_t occupancy = board.getOccupancy();

    // Pawns are directional.
    if (attackersColour == PieceColour::White)
    {
        if ((AttackTables::BlackPawnMask[square] & attackersPawns) != 0ULL)
            return true;
    }
    else
    {
        if ((AttackTables::WhitePawnMask[square] & attackersPawns) != 0ULL)
            return true;
    }

    // Knights are symmetric.
    if ((AttackTables::KnightMask[square] & attackersKnights) != 0ULL)
        return true;

    // Kings are symmetric.
    if ((AttackTables::KingMask[square] & attackersKing) != 0ULL)
        return true;

    // Rook-like attacks: rook or queen.
    if ((rookAttacksFromMask(square, occupancy) &
         (attackersRooks | attackersQueens)) != 0ULL)
    {
        return true;
    }

    // Bishop-like attacks: bishop or queen.
    if ((bishopAttacksFromMask(square, occupancy) &
         (attackersBishops | attackersQueens)) != 0ULL)
    {
        return true;
    }

    return false;
}

void pawnMoves(const BitBoard& board,int32_t square, PieceColour moverColour, std::vector<Move>& outMoves)
{

#if CHESS_DEBUG_BUILD
    const Piece debugPiece = board.getPieceAt(square);
    CHESS_ASSERT(!debugPiece.isEmpty() && debugPiece.m_type == PieceType::Pawn);
#endif

    const bool isWhite = moverColour == PieceColour::White;

    const uint64_t ownPieces = isWhite
                                   ? board.getWhiteOccupancy()
                                   : board.getBlackOccupancy();

    const uint64_t enemyPieces = isWhite
                                     ? board.getBlackOccupancy()
                                     : board.getWhiteOccupancy();

    const uint64_t occupancy = board.getOccupancy();
    const uint64_t emptySquares = ~occupancy;

    const uint64_t promoRank = isWhite
                   ? BitUtils::Rank8
                   : BitUtils::Rank1;

    const int32_t enPassantSquare = board.getEnPassantSquare();

    constexpr PieceType promos[] =
        {
            PieceType::Knight,
            PieceType::Bishop,
            PieceType::Rook,
            PieceType::Queen
        };

    outMoves.reserve(outMoves.size() + MaxMoves::Pawn);

    uint64_t bb = BitUtils::pawnMoves(
        square,
        isWhite,
        emptySquares,
        enemyPieces,
        board.getEnPassantSquare()
        ) & ~ownPieces;

    const uint64_t enemyPawns = isWhite
        ? board.getBitBoard(PieceType::Pawn, PieceColour::Black)
        : board.getBitBoard(PieceType::Pawn, PieceColour::White);


    while (bb)
    {
        const int32_t squareTo = BitUtils::popLSB(&bb);

        if (squareTo == enPassantSquare)
        {
            const uint64_t epMask = BitUtils::bitMask(squareTo);

            const uint64_t capturedPawnMask = isWhite
                  ? (epMask >> BitUtils::Board_Dim)
                  : (epMask << BitUtils::Board_Dim);

            if ((capturedPawnMask & enemyPawns) == 0ULL)
                continue;
        }

        if (BitUtils::hasBit(promoRank, squareTo))
        {
            for (PieceType pt : promos)
            {
                outMoves.emplace_back(
                    square,
                    squareTo,
                    Piece(pt, moverColour)
                    );
            }
        }
        else
        {
            outMoves.emplace_back(
                square,
                squareTo,
                Piece()
                );
        }
    }
}


void knightMoves(const BitBoard& board, int32_t square,  PieceColour moverColour,  std::vector<Move>& outMoves)
{
#if CHESS_DEBUG_BUILD
    const Piece debugPiece = board.getPieceAt(square);
    CHESS_ASSERT(!debugPiece.isEmpty() && debugPiece.m_type == PieceType::Knight);
#endif

    uint64_t ownPieces = moverColour == PieceColour::White?
            board.getWhiteOccupancy():
            board.getBlackOccupancy();

    uint64_t targets = AttackTables::KnightMask[square] & ~ownPieces;

    outMoves.reserve(outMoves.size() + MaxMoves::Knight);

    while(targets)
    {
        int32_t squareTo = BitUtils::popLSB(&targets);
        outMoves.emplace_back(square, squareTo, Piece());
    }
}


static void addQuietTargets(int32_t from,
                            uint64_t targets,
                            std::vector<Move>& outMoves)
{
    while (targets)
    {
        const int32_t to = BitUtils::popLSB(&targets);
        outMoves.emplace_back(from, to, Piece());
    }
}

void rookMoves(const BitBoard& board, int32_t square, PieceColour moverColour, std::vector<Move>& outMoves)
{
#if CHESS_DEBUG_BUILD
    const Piece debugPiece = board.getPieceAt(square);
    CHESS_ASSERT(!debugPiece.isEmpty() && debugPiece.m_type == PieceType::Rook);
#endif

    const uint64_t ownPieces = moverColour == PieceColour::White
                                   ? board.getWhiteOccupancy()
                                   : board.getBlackOccupancy();

    outMoves.reserve(outMoves.size() + MaxMoves::Rook);

    uint64_t targets = rookAttacksFromMask(square, board.getOccupancy()) & ~ownPieces;
    addQuietTargets(square, targets, outMoves);
}


void bishopMoves(const BitBoard& board, int32_t square,PieceColour moverColour, std::vector<Move>& outMoves)
{
#if CHESS_DEBUG_BUILD
    const Piece debugPiece = board.getPieceAt(square);
    CHESS_ASSERT(!debugPiece.isEmpty() && debugPiece.m_type == PieceType::Bishop);
#endif

    const uint64_t ownPieces = moverColour == PieceColour::White
                                   ? board.getWhiteOccupancy()
                                   : board.getBlackOccupancy();

    outMoves.reserve(outMoves.size() + MaxMoves::Bishop);

    uint64_t targets = bishopAttacksFromMask(square, board.getOccupancy()) & ~ownPieces;

    addQuietTargets(square, targets, outMoves);
}


void queenMoves(const BitBoard& board, int32_t square, PieceColour moverColour, std::vector<Move>& outMoves)
{
#if CHESS_DEBUG_BUILD
    const Piece debugPiece = board.getPieceAt(square);
    CHESS_ASSERT(!debugPiece.isEmpty() && debugPiece.m_type == PieceType::Queen);
#endif

    const uint64_t ownPieces = moverColour == PieceColour::White
                                   ? board.getWhiteOccupancy()
                                   : board.getBlackOccupancy();

    outMoves.reserve(outMoves.size() + MaxMoves::Queen);

    const uint64_t occupancy = board.getOccupancy();

    uint64_t targets = (rookAttacksFromMask(square, occupancy) |
                        bishopAttacksFromMask(square, occupancy))
                        & ~ownPieces;

   addQuietTargets(square, targets, outMoves);
}


static void kingCastlingMoves(const BitBoard& board,
                                     int32_t squareFrom,
                                     PieceColour kingColour,
                                     std::vector<Move>& outMoves)
{   
    const bool isWhite = kingColour == PieceColour::White;

    const CastlingRight castlingRights = board.getCastling();

    const bool hasKingSide = isWhite
                                 ? hasFlag(castlingRights, CastlingRight::WhiteKingSide)
                                 : hasFlag(castlingRights, CastlingRight::BlackKingSide);

    const bool hasQueenSide = isWhite
                                  ? hasFlag(castlingRights, CastlingRight::WhiteQueenSide)
                                  : hasFlag(castlingRights, CastlingRight::BlackQueenSide);

    const int32_t kingStart = isWhite
                                  ? AttackTables::e1
                                  : AttackTables::e8;

    if (squareFrom != kingStart)
        return;

    const uint64_t occupancy = board.getOccupancy();

    const int32_t kingSideTarget = isWhite
                                       ? AttackTables::g1
                                       : AttackTables::g8;

    const int32_t queenSideTarget = isWhite
                                        ? AttackTables::c1
                                        : AttackTables::c8;

    const int32_t kingSideRookSquare = isWhite
                                           ? AttackTables::h1
                                           : AttackTables::h8;

    const int32_t queenSideRookSquare = isWhite
                                            ? AttackTables::a1
                                            : AttackTables::a8;

    const Piece kingSideRook = board.getPieceAt(kingSideRookSquare);
    const Piece queenSideRook = board.getPieceAt(queenSideRookSquare);

    const bool hasKingSideRook =
        !kingSideRook.isEmpty() &&
        kingSideRook.m_type == PieceType::Rook &&
        kingSideRook.m_colour == kingColour;

    const bool hasQueenSideRook =
        !queenSideRook.isEmpty() &&
        queenSideRook.m_type == PieceType::Rook &&
        queenSideRook.m_colour == kingColour;

    const uint64_t kingSideEmptyMask = isWhite
                                           ? (BitUtils::bitMask(AttackTables::f1) |
                                              BitUtils::bitMask(AttackTables::g1))
                                           : (BitUtils::bitMask(AttackTables::f8) |
                                              BitUtils::bitMask(AttackTables::g8));

    const uint64_t queenSideEmptyMask = isWhite
                                            ? (BitUtils::bitMask(AttackTables::d1) |
                                               BitUtils::bitMask(AttackTables::c1) |
                                               BitUtils::bitMask(AttackTables::b1))
                                            : (BitUtils::bitMask(AttackTables::d8) |
                                               BitUtils::bitMask(AttackTables::c8) |
                                               BitUtils::bitMask(AttackTables::b8));

    const int32_t fSquare = isWhite ? AttackTables::f1 : AttackTables::f8;
    const int32_t gSquare = isWhite ? AttackTables::g1 : AttackTables::g8;

    const int32_t dSquare = isWhite ? AttackTables::d1 : AttackTables::d8;
    const int32_t cSquare = isWhite ? AttackTables::c1 : AttackTables::c8;


    if (hasKingSide &&
        hasKingSideRook &&
        ((occupancy & kingSideEmptyMask) == 0ULL) &&
        !MoveGeneration::isSquareAttacked(board, kingStart, kingColour) &&
        !MoveGeneration::isSquareAttacked(board, fSquare,   kingColour) &&
        !MoveGeneration::isSquareAttacked(board, gSquare,   kingColour))
    {
        outMoves.emplace_back(squareFrom, kingSideTarget, Piece());
    }


    if (hasQueenSide &&
        hasQueenSideRook &&
        ((occupancy & queenSideEmptyMask) == 0ULL) &&
        !MoveGeneration::isSquareAttacked(board, kingStart, kingColour) &&
        !MoveGeneration::isSquareAttacked(board, dSquare,   kingColour) &&
        !MoveGeneration::isSquareAttacked(board, cSquare,   kingColour))
    {
        outMoves.emplace_back(squareFrom, queenSideTarget, Piece());
    }
}



/* older version loop based
static bool isSquareAttackedBy(int32_t square,
                               PieceColour attackersColour,
                               uint64_t attackersPawns,
                               uint64_t attackersKnights,
                               uint64_t attackersBishops,
                               uint64_t attackersRooks,
                               uint64_t attackersQueens,
                               uint64_t attackersKing,
                               uint64_t occupancy) noexcept
{  
    if (attackersColour == PieceColour::White)
    {
        if ((AttackTables::BlackPawnMask[square] & attackersPawns) != 0ULL)
            return true;
    }
    else if (attackersColour == PieceColour::Black)
    {
        if ((AttackTables::WhitePawnMask[square] & attackersPawns) != 0ULL)
            return true;
    }
    else
    {
        return false;
    }

    if ((AttackTables::KnightMask[square] & attackersKnights) != 0ULL)
        return true;

    if ((AttackTables::KingMask[square] & attackersKing) != 0ULL)
        return true;

    if ((BitUtils::rookAttacks(square, occupancy) &
         (attackersRooks | attackersQueens)) != 0ULL)
    {
        return true;
    }

    if ((BitUtils::bishopAttacks(square, occupancy) &
         (attackersBishops | attackersQueens)) != 0ULL)
    {
        return true;
    }

    return false;
}
*/


static bool isSquareAttackedBy(int32_t square,
                               PieceColour attackersColour,
                               uint64_t attackersPawns,
                               uint64_t attackersKnights,
                               uint64_t attackersBishops,
                               uint64_t attackersRooks,
                               uint64_t attackersQueens,
                               uint64_t attackersKing,
                               uint64_t occupancy) noexcept
{
#if CHESS_DEBUG_BUILD
    CHESS_ASSERT(BitUtils::isValidSquare(square));
#endif

    if (attackersColour == PieceColour::White)
    {
        if ((AttackTables::BlackPawnMask[square] & attackersPawns) != 0ULL)
            return true;
    }
    else if (attackersColour == PieceColour::Black)
    {
        if ((AttackTables::WhitePawnMask[square] & attackersPawns) != 0ULL)
            return true;
    }
    else
    {
        return false;
    }

    if ((AttackTables::KnightMask[square] & attackersKnights) != 0ULL)
        return true;

    if ((AttackTables::KingMask[square] & attackersKing) != 0ULL)
        return true;

    const uint64_t rookLikeAttackers = attackersRooks | attackersQueens;

    if ((rookAttacksFromMask(square, occupancy) & rookLikeAttackers) != 0ULL)
        return true;

    const uint64_t bishopLikeAttackers = attackersBishops | attackersQueens;

    if ((bishopAttacksFromMask(square, occupancy) & bishopLikeAttackers) != 0ULL)
        return true;

    return false;
}


static bool isKingTargetAttackedBy(const BitBoard& board,
                                            int32_t kingFrom,
                                            int32_t kingTo,
                                            PieceColour kingColour) noexcept
{
    if (!BitUtils::isValidSquare(kingFrom) ||
        !BitUtils::isValidSquare(kingTo))
    {
        return true;
    }

    const PieceColour attackersColour =
        kingColour == PieceColour::White
            ? PieceColour::Black
            : PieceColour::White;

    const uint64_t fromMask = BitUtils::bitMask(kingFrom);
    const uint64_t toMask   = BitUtils::bitMask(kingTo);

    uint64_t occupancy = board.getOccupancy();

    // Remove king from old square so it no longer blocks slider attacks.
    occupancy &= ~fromMask;

    // Remove captured enemy piece from target square, if any.
    occupancy &= ~toMask;

    // Place king on target square.
    occupancy |= toMask;

    uint64_t attackersPawns =
        board.getBitBoard(PieceType::Pawn, attackersColour) & ~toMask;

    uint64_t attackersKnights =
        board.getBitBoard(PieceType::Knight, attackersColour) & ~toMask;

    uint64_t attackersBishops =
        board.getBitBoard(PieceType::Bishop, attackersColour) & ~toMask;

    uint64_t attackersRooks =
        board.getBitBoard(PieceType::Rook, attackersColour) & ~toMask;

    uint64_t attackersQueens =
        board.getBitBoard(PieceType::Queen, attackersColour) & ~toMask;

    uint64_t attackersKing =
        board.getBitBoard(PieceType::King, attackersColour);

    return isSquareAttackedBy(
        kingTo,
        attackersColour,
        attackersPawns,
        attackersKnights,
        attackersBishops,
        attackersRooks,
        attackersQueens,
        attackersKing,
        occupancy
        );
}

void kingMoves(const BitBoard& board, int32_t square, PieceColour moverColour, std::vector<Move>& outMoves)
{
#if CHESS_DEBUG_BUILD
    const Piece debugPiece = board.getPieceAt(square);
    CHESS_ASSERT(!debugPiece.isEmpty() && debugPiece.m_type == PieceType::King);
#endif

    const bool isWhite = moverColour ==  PieceColour::White;

    const uint64_t ownPieces = isWhite
                                   ? board.getWhiteOccupancy()
                                   : board.getBlackOccupancy();

    uint64_t targets = AttackTables::KingMask[square] & ~ownPieces;

    outMoves.reserve(outMoves.size() + MaxMoves::King);

    while (targets)
    {
        const int32_t squareTo = BitUtils::popLSB(&targets);

        if (isKingTargetAttackedBy(board, square, squareTo, moverColour))
        {
            continue;
        }

        outMoves.emplace_back(square, squareTo, Piece());
    }
    kingCastlingMoves(board, square, moverColour, outMoves);
}



static void saveUndoState(const BitBoard& board, const Move& move, UndoMove& undo)
{
    undo.m_from = move.m_from;
    undo.m_to = move.m_to;

    undo.m_movedPiece = board.getPieceAt(undo.m_from);
    undo.m_capturedPiece = board.getPieceAt(undo.m_to);
    undo.m_capturedSquare = undo.m_to;

    undo.m_castlingRights = board.getCastling();
    undo.m_prevEnPassantSquare = board.getEnPassantSquare();
    undo.m_prevMover = board.getMoversColour();

    undo.m_wasEnPassant = false;
    undo.m_rookFromSquare = BitUtils::Invalid_Square;
    undo.m_rookToSquare = BitUtils::Invalid_Square;
    undo.m_rookPiece = Piece();


    undo.m_halfMoveClock = board.getHalfMoveClock();
    undo.m_fullMove = board.getFullMove();

    undo.m_boardHash = board.getHash();
}


static void updateCastlingRights(BitBoard& board,
                                 const Piece& movingPiece,
                                 int32_t fromSq,
                                 const UndoMove& undo)
{
    /*
     * Any king move removes both castling rights for that colour.
     */
    if (movingPiece.m_type == PieceType::King)
    {
        if (movingPiece.m_colour == PieceColour::White)
        {
            board.removeCastlingRight(
                CastlingRight::WhiteKingSide |
                CastlingRight::WhiteQueenSide
                );
        }
        else if (movingPiece.m_colour == PieceColour::Black)
        {
            board.removeCastlingRight(
                CastlingRight::BlackKingSide |
                CastlingRight::BlackQueenSide
                );
        }
    }

    /*
     * Moving a rook from its original square removes that side's castling right.
     */
    if (movingPiece.m_type == PieceType::Rook)
    {
        if (fromSq == AttackTables::a1)
        {
            board.removeCastlingRight(CastlingRight::WhiteQueenSide);
        }
        else if (fromSq == AttackTables::h1)
        {
            board.removeCastlingRight(CastlingRight::WhiteKingSide);
        }
        else if (fromSq == AttackTables::a8)
        {
            board.removeCastlingRight(CastlingRight::BlackQueenSide);
        }
        else if (fromSq == AttackTables::h8)
        {
            board.removeCastlingRight(CastlingRight::BlackKingSide);
        }
    }

    /*
     * Capturing a rook on its original square removes that side's castling right.
     */
    if (undo.m_capturedPiece.m_type == PieceType::Rook)
    {
        const int32_t capturedSq = undo.m_capturedSquare;

        if (capturedSq == AttackTables::a1)
        {
            board.removeCastlingRight(CastlingRight::WhiteQueenSide);
        }
        else if (capturedSq == AttackTables::h1)
        {
            board.removeCastlingRight(CastlingRight::WhiteKingSide);
        }
        else if (capturedSq == AttackTables::a8)
        {
            board.removeCastlingRight(CastlingRight::BlackQueenSide);
        }
        else if (capturedSq == AttackTables::h8)
        {
            board.removeCastlingRight(CastlingRight::BlackKingSide);
        }
    }
}



static void CommitCastling(const int32_t rankTo,
                    const PieceType movingType,
                    BitBoard& board,
                    const int32_t fileTo,
                    const PieceColour movingColour,
                    UndoMove& undo,
                    const int32_t fileFrom)
{
    if (movingType == PieceType::King &&
        std::abs(fileFrom - fileTo) == 2)
    {
        const int32_t kingRank = rankTo;

        if (fileTo == 6)
        {
            // King-side castling: rook h-file -> f-file.
            undo.m_rookFromSquare = kingRank * BitUtils::Board_Dim + 7;
            undo.m_rookToSquare   = kingRank * BitUtils::Board_Dim + 5;
        }
        else
        {
            // Queen-side castling: rook a-file -> d-file.
            undo.m_rookFromSquare = kingRank * BitUtils::Board_Dim + 0;
            undo.m_rookToSquare   = kingRank * BitUtils::Board_Dim + 3;
        }

        undo.m_rookPiece = board.getPieceAt(undo.m_rookFromSquare);

        const Piece rookPiece =
            board.popPieceAt(undo.m_rookFromSquare, false);

        if (!rookPiece.isEmpty() &&
            rookPiece.m_type == PieceType::Rook &&
            rookPiece.m_colour == movingColour)
        {
            board.setPieceAt(rookPiece, undo.m_rookToSquare, false);
        }
    }
}


static void commitEnPassantCapture(const int32_t fileFrom,
                            const PieceColour movingColour,
                            BitBoard& board,
                            const int32_t sqTo,
                            const int32_t fileTo,
                            const PieceType movingType,
                            UndoMove& undo,
                            const bool isWhite)
{
    if (movingType == PieceType::Pawn &&
        fileFrom != fileTo &&
        board.getPieceAt(sqTo).isEmpty() &&
        board.getEnPassantSquare() == sqTo)
    {
        const int32_t capturedSq = isWhite
                                       ? sqTo - BitUtils::Board_Dim
                                       : sqTo + BitUtils::Board_Dim;

        if (BitUtils::isValidSquare(capturedSq))
        {
            const Piece captured = board.getPieceAt(capturedSq);

            if (!captured.isEmpty() &&
                captured.m_type == PieceType::Pawn &&
                captured.m_colour != movingColour)
            {
                undo.m_wasEnPassant = true;
                undo.m_capturedPiece = captured;
                undo.m_capturedSquare = capturedSq;

                board.popPieceAt(capturedSq, false);
            }
        }
    }
}

static void updateEnPassantSquare(const int32_t fileFrom,
                                  const PieceType movingType,
                                  BitBoard& board,
                                  const int32_t rankFrom,
                                  const int32_t rankTo)
{
    if (movingType == PieceType::Pawn &&
        std::abs(rankTo - rankFrom) == 2)
    {
        const int32_t epRank = (rankFrom + rankTo) / 2;
        const int32_t epSq = epRank * BitUtils::Board_Dim + fileFrom;

        board.setEnPassantSquare(epSq);
    }
    else
    {
        board.setEnPassantSquare(BitUtils::Invalid_Square);
    }
}

static void movePiece(BitBoard& board,
                      const Move& move,
                      const int32_t sqFrom,
                      const int32_t sqTo,
                      const Piece movingPiece)
{
    board.popPieceAt(sqFrom, false);

    if (move.m_promo.isNotEmpty())
    {
        board.setPieceAt(move.m_promo, sqTo, false);
    }
    else
    {
        board.setPieceAt(movingPiece, sqTo, false);
    }
}

static void updatehalfMoveClock(UndoMove& undo,
                                const PieceType movingType,
                                BitBoard& board)
{
    if (movingType == PieceType::Pawn ||
        undo.m_capturedPiece.isNotEmpty())
    {
        board.setHalfMoveClock(0);
    }
    else
    {
        board.setHalfMoveClock(board.getHalfMoveClock() + 1);
    }
}

static void normalCalpture(BitBoard& board,
                           UndoMove& undo,
                           const int32_t sqTo)
{
    if (!undo.m_wasEnPassant &&
        undo.m_capturedPiece.isNotEmpty())
    {
        board.popPieceAt(sqTo, false);
    }
}

static void updateFullMove(BitBoard& board)
{
    if (board.getMoversColour() == PieceColour::Black)
    {
        board.setFullMove(board.getFullMove() + 1);
    }
}

bool applyMove(BitBoard& board, const Move& move, UndoMove& undo) noexcept
{
    // Save everything needed to undo this move.
    saveUndoState(board, move, undo);

    const int32_t sqFrom = move.m_from;
    const int32_t sqTo   = move.m_to;

    if (!BitUtils::isValidSquare(sqFrom) ||
        !BitUtils::isValidSquare(sqTo))
    {
        return false;
    }

    const int32_t fileFrom = BitUtils::toFile(sqFrom);
    const int32_t fileTo   = BitUtils::toFile(sqTo);

    const int32_t rankFrom = BitUtils::toRank(sqFrom);
    const int32_t rankTo   = BitUtils::toRank(sqTo);

    const Piece movingPiece = undo.m_movedPiece;

    if (movingPiece.isEmpty())
    {
        return false;
    }

    const PieceColour movingColour = movingPiece.m_colour;
    const PieceType movingType = movingPiece.m_type;
    const bool isWhite = movingColour == PieceColour::White;

    // Handle en passant capture before moving the pawn.
    commitEnPassantCapture(fileFrom, movingColour, board, sqTo, fileTo, movingType, undo, isWhite);

    // Reset or increment the halfmove clock.
    updatehalfMoveClock(undo, movingType, board);

    // Remove a normally captured piece from the target square.
    normalCalpture(board, undo, sqTo);

    // Move the piece, or place the promoted piece.
    movePiece(board, move, sqFrom, sqTo, movingPiece);

    // Move the rook if this is castling.
    CommitCastling(rankTo, movingType, board, fileTo, movingColour, undo, fileFrom);

    // Remove castling rights affected by this move.
    updateCastlingRights(board, movingPiece, sqFrom, undo);

    // Set or clear the en passant target square.
    updateEnPassantSquare(fileFrom, movingType, board, rankFrom, rankTo);

    // Increment fullmove number after black moves.
    updateFullMove(board);

    // Rebuild derived board state.
    board.updateOccupancy();
    board.toggleMover();
    board.updateHash();

    return true;
}



void UnApplyMove(BitBoard& board, const UndoMove& undo) noexcept
{
    const int32_t sqFrom = undo.m_from;
    const int32_t sqTo   = undo.m_to;

    // Restore previous side to move.
    board.setMoversColour(undo.m_prevMover);

    // Clear squares affected by the move.
    board.clearSquareAt(sqFrom, false);
    board.clearSquareAt(sqTo, false);

    // Restore moved piece to its original square.
    board.setPieceAt(undo.m_movedPiece, sqFrom, false);

    // Restore captured piece, including en passant captures.
    if (undo.m_capturedPiece.isNotEmpty() &&
        undo.m_capturedSquare != BitUtils::Invalid_Square)
    {
        board.setPieceAt(
            undo.m_capturedPiece,
            undo.m_capturedSquare,
            false
            );
    }

    // Restore rook if the move was castling.
    if (undo.m_rookFromSquare != BitUtils::Invalid_Square &&
        undo.m_rookToSquare != BitUtils::Invalid_Square)
    {
        board.clearSquareAt(undo.m_rookToSquare, false);

        if (undo.m_rookPiece.isNotEmpty())
        {
            board.setPieceAt(
                undo.m_rookPiece,
                undo.m_rookFromSquare,
                false
                );
        }
    }

    // Restore reversible board state.
    board.setCastling(undo.m_castlingRights);
    board.setEnPassantSquare(undo.m_prevEnPassantSquare);
    board.setFullMove(undo.m_fullMove);
    board.setHalfMoveClock(undo.m_halfMoveClock);

    // Rebuild derived state.
    board.updateOccupancy();    
    board.setHash(undo.m_boardHash);
}



void generateMoves(const BitBoard& board, std::vector<Move>& outMoves, uint32_t maxMoves)
{
    const PieceColour mover = board.getMoversColour();

    if (mover != PieceColour::White &&
        mover != PieceColour::Black)
    {
        return;
    }

    outMoves.reserve(outMoves.size() + maxMoves);

    uint64_t pawns   = board.getBitBoard(PieceType::Pawn,   mover);
    uint64_t knights = board.getBitBoard(PieceType::Knight, mover);
    uint64_t bishops = board.getBitBoard(PieceType::Bishop, mover);
    uint64_t rooks   = board.getBitBoard(PieceType::Rook,   mover);
    uint64_t queens  = board.getBitBoard(PieceType::Queen,  mover);
    uint64_t kings   = board.getBitBoard(PieceType::King,   mover);

    auto handleBitboard =
        [&](uint64_t bb, auto moveFunc)
    {
        while (bb)
        {
            const int32_t sq = BitUtils::popLSB(&bb);

            if (sq == BitUtils::Invalid_Square)
                break;

            moveFunc(board, sq, mover, outMoves);
        }
    };

    handleBitboard(pawns,   MoveGeneration::pawnMoves);
    handleBitboard(knights, MoveGeneration::knightMoves);
    handleBitboard(bishops, MoveGeneration::bishopMoves);
    handleBitboard(rooks,   MoveGeneration::rookMoves);
    handleBitboard(queens,  MoveGeneration::queenMoves);
    handleBitboard(kings,   MoveGeneration::kingMoves);
}


void generateLegalMoves(BitBoard& board,std::vector<Move>& outMoves, uint32_t maxMoves)
{
    std::vector<Move> pseudoMoves;
    pseudoMoves.reserve(maxMoves);

    generateMoves(board, pseudoMoves, maxMoves);

    outMoves.clear();
    outMoves.reserve(maxMoves);

    for (const Move& move : pseudoMoves)
    {
        UndoMove undo;

        const PieceColour moverColour = board.getMoversColour();

        if (!applyMove(board, move, undo))
            continue;

        const uint64_t kingBB =
            board.getBitBoard(PieceType::King, moverColour);

        const int32_t kingSquare = BitUtils::indexLSB(kingBB);

        bool legal = true;

        if (kingSquare == BitUtils::Invalid_Square)
        {
            legal = false;
        }
        else if (MoveGeneration::isSquareAttacked(board, kingSquare, moverColour))
        {
            legal = false;
        }

        UnApplyMove(board, undo);

        if (legal)
        {
            outMoves.push_back(move);
        }
    }
}


void generateMovesFromSquare(BitBoard& board, int32_t square, std::vector<Move>& outMoves, uint32_t maxMoves)
{
    if (!BitUtils::isValidSquare(square))
        return;

    const Piece piece = board.getPieceAt(square);

    if (piece.isEmpty() ||
        piece.m_colour != board.getMoversColour())
    {
        return;
    }

    outMoves.reserve(outMoves.size() + maxMoves);

    switch (piece.m_type)
    {
    case PieceType::Pawn:
        pawnMoves(board, square, piece.m_colour, outMoves);
        break;

    case PieceType::Knight:
        knightMoves(board, square, piece.m_colour, outMoves);
        break;

    case PieceType::Bishop:
        bishopMoves(board, square, piece.m_colour, outMoves);
        break;

    case PieceType::Rook:
        rookMoves(board, square, piece.m_colour, outMoves);
        break;

    case PieceType::Queen:
        queenMoves(board, square, piece.m_colour, outMoves);
        break;

    case PieceType::King:
        kingMoves(board, square, piece.m_colour, outMoves);
        break;

    default:
        break;
    }
}


void generateLegalMovesFromSquare(BitBoard& board, int32_t square, std::vector<Move>& outMoves, uint32_t maxMoves)
{
    outMoves.clear();

    if (!BitUtils::isValidSquare(square))
        return;

    const PieceColour moverColour = board.getMoversColour();

    std::vector<Move> pseudoMoves;
    pseudoMoves.reserve(maxMoves);

    generateMovesFromSquare(board, square, pseudoMoves, maxMoves);

    outMoves.reserve(maxMoves);

    for (const Move& move : pseudoMoves)
    {
        UndoMove undo;

        if (!applyMove(board, move, undo))
            continue;

        const uint64_t kingBB =
            board.getBitBoard(PieceType::King, moverColour);

        const int32_t kingSquare = BitUtils::indexLSB(kingBB);

        const bool legal =
            kingSquare != BitUtils::Invalid_Square &&
            !MoveGeneration::isSquareAttacked(board, kingSquare, moverColour);

        UnApplyMove(board, undo);

        if (legal)
            outMoves.push_back(move);
    }
}

bool tryCommitMove(BitBoard& board,
                int32_t from,
                int32_t to,
                Piece promo,
                const std::vector<Move>& legalMoves,
                std::vector<UndoMove>& undoLog)
{
    if (!BitUtils::isValidSquare(from) ||
        !BitUtils::isValidSquare(to))
    {
        return false;
    }

    const Piece piece = board.getPieceAt(from);

    if (piece.isEmpty())
        return false;

    if (piece.m_colour != board.getMoversColour())
        return false;

    const Move* chosenMove = nullptr;

    for (const Move& move : legalMoves)
    {
        if (move.m_from != from || move.m_to != to)
            continue;

        const bool legalMoveIsPromotion = move.m_promo.isNotEmpty();
        const bool requestedPromotion   = promo.isNotEmpty();


        if (legalMoveIsPromotion)
        {
            if (!requestedPromotion)
                continue;

            if (move.m_promo.m_type != promo.m_type)
                continue;

            chosenMove = &move;
            break;
        }


        if (requestedPromotion)
            continue;

        chosenMove = &move;
        break;
    }

    if (!chosenMove)
        return false;

    UndoMove undo;

    if (!applyMove(board, *chosenMove, undo))
        return false;

    undoLog.push_back(undo);
    return true;
}

bool tryCommitMove(BitBoard& board,
                   int32_t from,
                   int32_t to,
                   PieceType promoType,
                   const std::vector<Move>& legalMoves,
                   std::vector<UndoMove>& undoLog)
{
    Piece promo;

    if (promoType != PieceType::None)
    {
        promo = Piece(promoType, board.getMoversColour());
    }

    return tryCommitMove(
        board,
        from,
        to,
        promo,
        legalMoves,
        undoLog
        );
}

bool commitMove(BitBoard& board,
                int32_t from,
                int32_t to,
                PieceType promoType)
{
    Piece promo;

    if (promoType != PieceType::None)
    {
        promo = Piece(promoType, board.getMoversColour());
    }

    return commitMove(board, from, to, promo);
}

bool commitMove(BitBoard& board,
                int32_t from,
                int32_t to,
                Piece promo)
{
    if (!BitUtils::isValidSquare(from) ||
        !BitUtils::isValidSquare(to))
    {
        return false;
    }

    const Piece piece = board.getPieceAt(from);

    if (piece.isEmpty())
        return false;

    if (piece.m_colour != board.getMoversColour())
        return false;

    const Move* chosenMove = nullptr;
    std::vector<Move> legalMoves;

    generateLegalMovesFromSquare(board, from,legalMoves);

    for (const Move& move : legalMoves)
    {
        if (move.m_from != from || move.m_to != to)
            continue;

        const bool legalMoveIsPromotion = move.m_promo.isNotEmpty();
        const bool requestedPromotion   = promo.isNotEmpty();


        if (legalMoveIsPromotion)
        {
            if (!requestedPromotion)
                continue;

            if (move.m_promo.m_type != promo.m_type)
                continue;

            chosenMove = &move;
            break;
        }

        if (requestedPromotion)
            continue;

        chosenMove = &move;
        break;
    }

    if (!chosenMove)
        return false;

    UndoMove undo;

    if (!applyMove(board, *chosenMove, undo))
        return false;

    return true;
}


bool commitMoveUci(BitBoard& board, const std::string& uci)
{
    if(uci.empty() || uci == "none" || uci == "(none)")
    {
        return false;
    }

    const Move move = UciUtils::uciToMove(uci);
    return commitMove(board, move.m_from, move.m_to, move.m_promo.m_type);
}


bool rollback(BitBoard& board,
              std::vector<UndoMove>& undoLog,
              uint32_t moveCount)
{
    if (moveCount == 0)
        return true;

    if (undoLog.empty() || moveCount > undoLog.size())
        return false;

    for (uint32_t i = 0; i < moveCount; ++i)
    {
        const UndoMove undo = undoLog.back();

        UnApplyMove(board, undo);

        undoLog.pop_back();
    }
    return true;
}


bool rollback(BitBoard& board,
              std::vector<UndoMove>& undoLog,
              std::vector<Hash_128>& positionHistory,
              uint32_t moveCount)
{
    if (moveCount == 0)
        return true;

    if (moveCount > undoLog.size())
        return false;

    for (uint32_t i = 0; i < moveCount; ++i)
    {
        const UndoMove undo = undoLog.back();

        MoveGeneration::UnApplyMove(board, undo);

        undoLog.pop_back();

        if (!positionHistory.empty())
            positionHistory.pop_back();
    }

    return true;
}


static bool hasAnyLegalMoves(BitBoard& board)
{
    std::vector<Move> pseudoMoves;
    pseudoMoves.reserve(MaxMoves::Total_Per_Side);

    MoveGeneration::generateMoves(board, pseudoMoves);

    const PieceColour mover = board.getMoversColour();

    for (const Move& move : pseudoMoves)
    {
        UndoMove undo;

        if (!MoveGeneration::applyMove(board, move, undo))
            continue;

        const int32_t kingSq = BitUtils::indexLSB(
            board.getBitBoard(PieceType::King, mover)
            );

        const bool legal =
            kingSq != BitUtils::Invalid_Square &&
            !MoveGeneration::isSquareAttacked(board, kingSq, mover);

        MoveGeneration::UnApplyMove(board, undo);

        if (legal)
            return true;
    }

    return false;
}


static bool isCheckmate(const BitBoard& board)
{
    BitBoard cpy = board;
    const PieceColour mover = cpy.getMoversColour();

    const int32_t kingSq = BitUtils::indexLSB(
        cpy.getBitBoard(PieceType::King, mover)
        );

    if (kingSq == BitUtils::Invalid_Square)
        return false;

    if (!MoveGeneration::isSquareAttacked(cpy, kingSq, mover))
        return false;

    return !hasAnyLegalMoves(cpy);
}


static bool isStalemate(const BitBoard& board)
{
    BitBoard cpy = board;
    const PieceColour mover = cpy.getMoversColour();

    const int32_t kingSq = BitUtils::indexLSB(
        cpy.getBitBoard(PieceType::King, mover)
        );

    if (kingSq == BitUtils::Invalid_Square)
        return false;

    if (MoveGeneration::isSquareAttacked(cpy, kingSq, mover))
        return false;

    return !hasAnyLegalMoves(cpy);
}

static bool isThreefoldRepetition(const std::vector<Hash_128>& history,
                                  const Hash_128& currHash,
                                  int32_t halfMoveCount)
{
    int32_t count =0;

    const int32_t start = static_cast<int32_t>(history.size()) - 1 - halfMoveCount;

    const int32_t begin = start > 0 ? start : 0;

    for (int32_t i = static_cast<int32_t>(history.size()) - 1;
         i >= begin;
         --i)
    {
        if (history[i] == currHash)
        {
            ++count;

            if (count >= 3)
                return true;
        }
    }

    return false;
}

static bool hasInsufficientMaterial(const BitBoard& board) noexcept
{
    const uint64_t whitePawns = board.getBitBoard(PieceType::Pawn, PieceColour::White);
    const uint64_t blackPawns = board.getBitBoard(PieceType::Pawn, PieceColour::Black);

    const uint64_t whiteRooks = board.getBitBoard(PieceType::Rook, PieceColour::White);
    const uint64_t blackRooks = board.getBitBoard(PieceType::Rook, PieceColour::Black);

    const uint64_t whiteQueens = board.getBitBoard(PieceType::Queen, PieceColour::White);
    const uint64_t blackQueens = board.getBitBoard(PieceType::Queen, PieceColour::Black);


    if((whitePawns | blackPawns |
        whiteRooks | blackRooks |
        whiteQueens | blackQueens) != 0ULL)
    {
        return false;
    }

    const uint64_t whiteBishops = board.getBitBoard(PieceType::Bishop, PieceColour::White);
    const uint64_t blackBishops = board.getBitBoard(PieceType::Bishop, PieceColour::Black);

    const uint64_t whiteKnights = board.getBitBoard(PieceType::Knight, PieceColour::White);
    const uint64_t blackKnights = board.getBitBoard(PieceType::Knight, PieceColour::Black);

    const int32_t whiteBishopCount = BitUtils::popCount(whiteBishops);
    const int32_t blackBishopCount = BitUtils::popCount(blackBishops);

    const int32_t whiteKnightCount  = BitUtils::popCount(whiteKnights);
    const int32_t blackKnightCount = BitUtils::popCount(blackKnights);

    const int32_t minorCount = whiteBishopCount + blackBishopCount +
                               whiteKnightCount + blackKnightCount;

    if(minorCount == 0)
    {
        return true;
    }

    if(minorCount == 1)
    {
        return true;
    }

    if (minorCount == 2 &&
        whiteBishopCount == 1 &&
        blackBishopCount == 1 &&
        whiteKnightCount == 0 &&
        blackKnightCount == 0)
    {
        const int whiteBishopSq = BitUtils::indexLSB(whiteBishops);
        const int blackBishopSq = BitUtils::indexLSB(blackBishops);

        const bool whiteBishopLight =
            ((BitUtils::toRank(whiteBishopSq) + BitUtils::toFile(whiteBishopSq)) & 1) == 0;

        const bool blackBishopLight =
            ((BitUtils::toRank(blackBishopSq) + BitUtils::toFile(blackBishopSq)) & 1) == 0;

        return whiteBishopLight == blackBishopLight;
    }

    return false;
}



GameStatus endGameStatus(const BitBoard& board, const std::vector<Hash_128>& hashList)
{
    if (isCheckmate(board))
    {
        return board.getMoversColour() == PieceColour::White
                   ? GameStatus::BlackWonByCheckmate
                   : GameStatus::WhiteWonByCheckmate;
    }

    if (isStalemate(board))
        return GameStatus::DrawByStalemate;

    if (board.getHalfMoveClock() >= 100)
        return GameStatus::DrawByFiftyMoveRule;

    if (isThreefoldRepetition(hashList, board.getHash(), board.getFullMove()))
        return GameStatus::DrawByThreefoldRepetition;

    if (hasInsufficientMaterial(board))
        return GameStatus::DrawByInsufficientMaterial;

    return GameStatus::Ongoing;
}


} // namespace MoveGeneration



} // namespace ChessCore