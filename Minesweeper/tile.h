#pragma once

struct Tile {
    bool isMine;
    bool isRevealed;
    bool isFlagged;
    int NeighbourMines;
};