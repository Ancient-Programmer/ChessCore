#include "attacktables.h"

namespace ChessCore
{
namespace AttackTables
{

uint64_t KingMask[64] = {};
uint64_t KnightMask[64] = {};
uint64_t WhitePawnMask[64] = {};
uint64_t BlackPawnMask[64] = {};

uint64_t RookRayMask[64][4] = {};
uint64_t BishopRayMask[64][4] = {};


} // namespace AttackTables
} // namespace ChessCore

