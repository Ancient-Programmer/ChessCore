#ifndef CHESSTESTS_H
#define CHESSTESTS_H

#include <cstdint>
#include "bitboard.h"


namespace ChessCore
{

void perftDivide(BitBoard& board, int depth);
uint64_t perftFast(BitBoard& board, int depth);
uint64_t perft(BitBoard& board, int depth);

}
#endif // CHESSTESTS_H
