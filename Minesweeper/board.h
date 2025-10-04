#pragma once

#include "tile.h"

struct GameBoard {
    Tile** Grid;
    int Width;
    int Height;
    int MineCount;
};