#ifndef MOVEGENERATION_H
#define MOVEGENERATION_H



#include <vector>
#include "bitboard.h"



namespace ChessCore
{




namespace MoveGeneration
{

/**
 * Initializes all precomputed attack tables.
 *
 * Must be called once before move generation is used.
 * This usually fills king, knight, pawn, rook ray, and bishop ray masks.
 */
void initAttackTables() noexcept;

/**
 * Generates rook attacks from a square using precomputed rook ray masks
 * and the current board occupancy.
 *
 * The returned attacks stop at the first blocker in each rook direction.
 *
 * @param square Source square of the rook.
 * @param occupancy Current full board occupancy.
 * @return Bitboard of attacked squares.
 */
uint64_t rookAttacksFromMask(int32_t square, uint64_t occupancy) noexcept;

/**
 * Generates bishop attacks from a square using precomputed bishop ray masks
 * and the current board occupancy.
 *
 * The returned attacks stop at the first blocker in each bishop direction.
 *
 * @param square Source square of the bishop.
 * @param occupancy Current full board occupancy.
 * @return Bitboard of attacked squares.
 */
uint64_t bishopAttacksFromMask(int32_t square, uint64_t occupancy) noexcept;

/**
 * Generates queen attacks from a square.
 *
 * This is usually rook attacks combined with bishop attacks.
 *
 * @param square Source square of the queen.
 * @param occupancy Current full board occupancy.
 * @return Bitboard of attacked squares.
 */
uint64_t queenAttacksFromMask(int32_t square, uint64_t occupancy) noexcept;

/**
 * Generates king attacks from a square using the precomputed king mask.
 *
 * The occupancy parameter is currently not required for basic king attacks,
 * but may be kept for interface symmetry or future filtering.
 *
 * @param square Source square of the king.
 * @param occupancy Current board occupancy.
 * @return Bitboard of attacked squares.
 */
uint64_t kingAttacksFromMask(int32_t square, uint64_t occupancy) noexcept;

/**
 * Checks whether a square is attacked by the opponent of the given colour.
 *
 * For example, if colour is White, this checks whether the square is attacked
 * by black pieces.
 *
 * @param Board Board position to inspect.
 * @param square Square to test.
 * @param colour Colour of the side being attacked.
 * @return true if the square is attacked, false otherwise.
 */
bool isSquareAttacked(const BitBoard& Board,
                      int32_t square,
                      PieceColour colour) noexcept;

/**
 * Generates pseudo-legal pawn moves from a given square.
 *
 * This includes single pushes, double pushes, captures, en passant targets,
 * and promotion moves. It does not guarantee that the move leaves the king safe.
 *
 * @param board Current board position.
 * @param square Source square of the pawn.
 * @param moverColour Colour of the moving side.
 * @param outMoves Output move list.
 */
void pawnMoves(const BitBoard& board,
               int32_t square,
               PieceColour moverColour,
               std::vector<Move>& outMoves);

/**
 * Generates pseudo-legal knight moves from a given square.
 *
 * Knights use precomputed attack masks and are filtered only by own occupancy.
 *
 * @param board Current board position.
 * @param square Source square of the knight.
 * @param moverColour Colour of the moving side.
 * @param outMoves Output move list.
 */
void knightMoves(const BitBoard& board,
                 int32_t square,
                 PieceColour moverColour,
                 std::vector<Move>& outMoves);

/**
 * Generates pseudo-legal queen moves from a given square.
 *
 * Queen moves are generated as rook-like attacks plus bishop-like attacks.
 *
 * @param board Current board position.
 * @param square Source square of the queen.
 * @param moverColour Colour of the moving side.
 * @param outMoves Output move list.
 */
void queenMoves(const BitBoard& board,
                int32_t square,
                PieceColour moverColour,
                std::vector<Move>& outMoves);

/**
 * Generates pseudo-legal rook moves from a given square.
 *
 * Rook moves are blocker-aware and stop at the first piece in each direction.
 *
 * @param board Current board position.
 * @param square Source square of the rook.
 * @param moverColour Colour of the moving side.
 * @param outMoves Output move list.
 */
void rookMoves(const BitBoard& board,
               int32_t square,
               PieceColour moverColour,
               std::vector<Move>& outMoves);

/**
 * Generates pseudo-legal bishop moves from a given square.
 *
 * Bishop moves are blocker-aware and stop at the first piece in each diagonal.
 *
 * @param board Current board position.
 * @param square Source square of the bishop.
 * @param moverColour Colour of the moving side.
 * @param outMoves Output move list.
 */
void bishopMoves(const BitBoard& board,
                 int32_t square,
                 PieceColour moverColour,
                 std::vector<Move>& outMoves);

/**
 * Generates king moves from a given square.
 *
 * Basic king moves are generated from precomputed king masks. Depending on
 * implementation, king target squares may also be filtered to avoid moving
 * into check. Castling moves may also be generated here.
 *
 * @param board Current board position.
 * @param square Source square of the king.
 * @param moverColour Colour of the moving side.
 * @param outMoves Output move list.
 */
void kingMoves(const BitBoard& board,
               int32_t square,
               PieceColour moverColour,
               std::vector<Move>& outMoves);

/**
 * Applies a move to the board and saves all required undo information.
 *
 * Handles normal moves, captures, promotions, en passant, castling,
 * castling-right updates, en passant-square updates, clocks, mover colour,
 * occupancy, and hash updates.
 *
 * This function assumes the move is pseudo-legal or legal enough to apply.
 * Legality should usually be checked by applying the move and verifying that
 * the mover's king is not attacked.
 *
 * @param board Board to modify.
 * @param move Move to apply.
 * @param undo Output undo state.
 * @return true if the move was applied, false otherwise.
 */
bool applyMove(BitBoard& board,
               const Move& move,
               UndoMove& undo) noexcept;

/**
 * Restores the board to the state before a previously applied move.
 *
 * Uses the stored UndoMove data to restore moved pieces, captured pieces,
 * castling rook movement, castling rights, en passant square, clocks,
 * mover colour, occupancy, and hash.
 *
 * @param board Board to restore.
 * @param undo Undo information produced by applyMove().
 */
void UnApplyMove(BitBoard& board,
                 const UndoMove& undo) noexcept;

/**
 * Generates all pseudo-legal moves for the current side to move.
 *
 * Pseudo-legal moves obey piece movement rules, captures, promotions,
 * castling structure, and en passant targets, but may still leave the king
 * in check.
 *
 * @param board Current board position.
 * @param outMoves Output move list.
 * @param maxMoves Maximum expected moves to reserve.
 */
void generateMoves(const BitBoard& board,
                   std::vector<Move>& outMoves,
                   uint32_t maxMoves = MaxMoves::Total_Per_Side);

/**
 * Generates all legal moves for the current side to move.
 *
 * This function normally generates pseudo-legal moves, applies each move,
 * checks whether the mover's king is attacked, then keeps only legal moves.
 *
 * @param board Board position to test. It is temporarily modified and restored.
 * @param outMoves Output legal move list.
 * @param maxMoves Maximum expected moves to reserve.
 */
void generateLegalMoves(BitBoard& board,
                        std::vector<Move>& outMoves,
                        uint32_t maxMoves = MaxMoves::Total_Per_Side);

/**
 * Generates pseudo-legal moves only from a specific square.
 *
 * Useful for UI selection and move highlighting. This function validates
 * that the selected square contains a piece belonging to the side to move,
 * then dispatches to the correct piece move generator.
 *
 * @param board Current board position.
 * @param square Source square.
 * @param outMoves Output move list.
 * @param maxMoves Maximum expected moves to reserve.
 */
void generateMovesFromSquare(const BitBoard& board,
                             int32_t square,
                             std::vector<Move>& outMoves,
                             uint32_t maxMoves = MaxMoves::Default_Per_Piece);

/**
 * Generates legal moves only from a specific square.
 *
 * Useful for UI move highlighting. This function generates pseudo-legal moves
 * from the selected square, applies each move, checks king safety, undoes the
 * move, and keeps only legal moves.
 *
 * @param board Board position to test. It is temporarily modified and restored.
 * @param square Source square.
 * @param outMoves Output legal move list.
 * @param maxMoves Maximum expected moves to reserve.
 */
void generateLegalMovesFromSquare(BitBoard& board,
                                  int32_t square,
                                  std::vector<Move>& outMoves,
                                  uint32_t maxMoves = MaxMoves::Default_Per_Piece);


/**
 * Attempts to commit a requested move using a previously generated legal move list.
 *
 * The function searches the provided legalMoves list for a move matching the
 * requested source square, target square, and promotion piece. If a matching
 * legal move is found, it is applied to the board and the produced undo state
 * is pushed into undoLog.
 *
 * This is useful when legal moves have already been generated, for example
 * after selecting a piece in the UI. No need to regenerate the whole circus,
 * mercifully.
 *
 * @param board Board to modify.
 * @param from Source square of the requested move.
 * @param to Target square of the requested move.
 * @param promo Promotion piece, or empty Piece() for non-promotion moves.
 * @param legalMoves Previously generated legal moves to validate against.
 * @param undoLog Undo history where the applied move undo state is stored.
 * @return true if the requested move was legal and committed, false otherwise.
 */
bool tryCommitMove(BitBoard& board,
                   int32_t from,
                   int32_t to,
                   Piece promo,
                   const std::vector<Move>& legalMoves,
                   std::vector<UndoMove>& undoLog);

bool tryCommitMove(BitBoard& board,
                   int32_t from,
                   int32_t to,
                   PieceType promoType,
                   const std::vector<Move>& legalMoves,
                   std::vector<UndoMove>& undoLog);

/**
 * Attempts to commit a requested move by generating legal moves internally.
 *
 * This is the convenient public move-entry function. It validates the requested
 * move against the legal moves available from the source square. If the move is
 * legal, it applies the move to the board.
 *
 * Use this when the caller does not already have a legal move list.
 *
 * @param board Board to modify.
 * @param from Source square of the requested move.
 * @param to Target square of the requested move.
 * @param promo Promotion piece, or empty Piece() for non-promotion moves.
 * @return true if the requested move was legal and committed, false otherwise.
 */
bool commitMove(BitBoard& board,
                int32_t from,
                int32_t to,
                Piece promo = Piece());

bool commitMove(BitBoard& board,
                int32_t from,
                int32_t to,
                PieceType promoType);

bool commitMoveUci(BitBoard& board,
                   const std::string& uci);

/**
 * Rolls back one or more committed moves using the undo history.
 *
 * The function restores the board by unapplying the latest moves stored in
 * undoLog. Each successfully restored move is removed from the undo history.
 *
 * @param board Board to restore.
 * @param undoLog Undo history containing previous move states.
 * @param moveCount Number of moves to roll back. Defaults to 1.
 * @return true if the rollback succeeded, false if there are not enough undo states.
 */
bool rollback(BitBoard& board,
              std::vector<UndoMove>& undoLog,
              uint32_t moveCount = 1);

bool rollback(BitBoard& board,
              std::vector<UndoMove>& undoLog,
              std::vector<Hash_128>& positionHistory,
              uint32_t moveCount = 1);

GameStatus endGameStatus(const BitBoard& board, const std::vector<Hash_128>& hashList);

} // namespace MoveGeneration

} // namespace ChessCore



#endif // MOVEGENERATION_H
