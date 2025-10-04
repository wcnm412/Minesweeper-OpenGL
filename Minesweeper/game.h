#pragma once

#include "tile.h"
#include "board.h"
#include "render.h"

void ClearGrid(GameBoard* Board);
void PopulateMines(GameBoard* Board);
void PopulateMines(GameBoard* Board, int gridX, int gridY);
int CountNeighbourMines(GameBoard* Board, int tileX, int tileY);
void SetNeighbourCounts(GameBoard* Board);
bool CheckWinCondition(GameBoard* Board);
void RevealAllMines(GameBoard* Board, GLuint VertexBuffer, int hitMineX, int hitMineY);
int CountFlags(GameBoard* Board);