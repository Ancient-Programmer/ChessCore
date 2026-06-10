#include "chesstests.h"
#include "movegeneration.h"
#include "bitutils.h"
#include "ChessDebug.h"

namespace ChessCore
{



uint64_t perft(BitBoard& board, int depth)
{
    if (depth == 0)
        return 1ULL;

    std::vector<Move> pseudoMoves;
    pseudoMoves.reserve(MaxMoves::Total_Per_Side);

    MoveGeneration::generateMoves(board, pseudoMoves, MaxMoves::Total_Per_Side);

    uint64_t nodes = 0ULL;

    const PieceColour mover = board.getMoversColour();

    for (const Move& move : pseudoMoves)
    {
        UndoMove undo;

        if (!MoveGeneration::applyMove(board, move, undo))
            continue;

        const uint64_t kingBB =
            board.getBitBoard(PieceType::King, mover);

        const int32_t kingSq = BitUtils::indexLSB(kingBB);

        const bool legal =
            kingSq != BitUtils::Invalid_Square &&
            !MoveGeneration::isSquareAttacked(board, kingSq, mover);

        if (legal)
        {
            if (depth == 1)
                nodes += 1ULL;
            else
                nodes += perft(board, depth - 1);
        }

        MoveGeneration::UnApplyMove(board, undo);
    }

    return nodes;
}




void perftDivide(BitBoard& board, int depth)
{
    std::vector<Move> moves;
    moves.reserve(MaxMoves::Total_Per_Side);

    MoveGeneration::generateMoves(board, moves);

    uint64_t total = 0ULL;

    for (const Move& move : moves)
    {
        UndoMove undo;

        if (!MoveGeneration::applyMove(board, move, undo))
            continue;

        const uint64_t nodes = perft(board, depth - 1);

        MoveGeneration::UnApplyMove(board, undo);

        total += nodes;

        CHESS_DEBUG_LOG(
            move.m_from,
            "->",
            move.m_to,
            " promo:",
            static_cast<int>(move.m_promo.m_type),
            " nodes:",
            nodes
            );
    }

    CHESS_INFO_LOG("Total nodes: ", total);
}


uint64_t perftFast(BitBoard& board, int depth)
{
    if (depth == 0)
        return 1ULL;

    std::vector<Move> moves;
    moves.reserve(MaxMoves::Total_Per_Side);

    MoveGeneration::generateMoves(board, moves, MaxMoves::Total_Per_Side);

    const PieceColour mover = board.getMoversColour();

    uint64_t nodes = 0ULL;

    for (const Move& move : moves)
    {
        UndoMove undo;

        if (!MoveGeneration::applyMove(board, move, undo))
            continue;

        const uint64_t kingBB =
            board.getBitBoard(PieceType::King, mover);

        const int32_t kingSq = BitUtils::indexLSB(kingBB);

        const bool legal =
            kingSq != BitUtils::Invalid_Square &&
            !MoveGeneration::isSquareAttacked(board, kingSq, mover);

        if (legal)
        {
            nodes += (depth == 1)
            ? 1ULL
            : perftFast(board, depth - 1);
        }

        MoveGeneration::UnApplyMove(board, undo);
    }

    return nodes;
}

} // namespace ChessCore