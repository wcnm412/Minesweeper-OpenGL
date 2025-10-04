#include "game.h"
#include <random>

static std::random_device rd;
static std::mt19937 gen(rd());

void ClearGrid(GameBoard* Board)
{
    for (int x{ 0 }; x < Board->Width; ++x) {
        for (int y{ 0 }; y < Board->Height; ++y) {
            Board->Grid[x][y].isMine = false;
            Board->Grid[x][y].isRevealed = false;
            Board->Grid[x][y].isFlagged = false;
            Board->Grid[x][y].NeighbourMines = 0;
        }
    }
}

void PopulateMines(GameBoard* Board)
{
    std::uniform_int_distribution<> distX(0, Board->Width - 1);
    std::uniform_int_distribution<> distY(0, Board->Height - 1);

    int RemainingMines{ Board->MineCount };
    while (RemainingMines > 0)
    {
        int x = distX(gen);
        int y = distY(gen);

        if (!Board->Grid[x][y].isMine)  // If not already a mine
        {
            Board->Grid[x][y].isMine = true;
            --RemainingMines;
        }
    }
}

void PopulateMines(GameBoard* Board, int gridX, int gridY)
{
    std::uniform_int_distribution<> distX(0, Board->Width - 1);
    std::uniform_int_distribution<> distY(0, Board->Height - 1);

    int RemainingMines{ Board->MineCount };
    while (RemainingMines > 0)
    {
        int x = distX(gen);
        int y = distY(gen);
        if (x >= gridX - 1 && x <= gridX + 1 &&
            y >= gridY - 1 && y <= gridY + 1)
        {
            continue;
        }
        if (!Board->Grid[x][y].isMine)  // If not already a mine
        {
            Board->Grid[x][y].isMine = true;
            --RemainingMines;
        }
    }
}

int CountNeighbourMines(GameBoard* Board, int tileX, int tileY)
{
    int count{ 0 };
    for (int offsetX{ -1 }; offsetX <= 1; ++offsetX)
    {
        for (int offsetY{ -1 }; offsetY <= 1; ++offsetY)
        {
            if (offsetX == 0 && offsetY == 0)
            {
                continue;
            }

            int neighbourX{ tileX + offsetX };
            int neighbourY{ tileY + offsetY };

            // Check bounds
            if (neighbourX >= 0 && neighbourX < Board->Width &&
                neighbourY >= 0 && neighbourY < Board->Height)
            {
                if (Board->Grid[neighbourX][neighbourY].isMine)
                {
                    ++count;
                }
            }
        }
    }
    return count;
}

void SetNeighbourCounts(GameBoard* Board)
{
    for (int x{ 0 }; x < Board->Width; ++x)
    {
        for (int y{ 0 }; y < Board->Height; ++y)
        {
            if (!Board->Grid[x][y].isMine)
            {
                Board->Grid[x][y].NeighbourMines = CountNeighbourMines(Board, x, y);
            }
        }
    }
}

bool CheckWinCondition(GameBoard* Board)
{
    for (int x{ 0 }; x < Board->Width; ++x)
    {
        for (int y{ 0 }; y < Board->Height; ++y)
        {
            if (!Board->Grid[x][y].isMine && !Board->Grid[x][y].isRevealed) return false;
        }
    }
    return true;
}

void RevealAllMines(GameBoard* Board, GLuint VertexBuffer, int hitMineX, int hitMineY)
{
    for (int x{ 0 }; x < Board->Width; ++x)
    {
        for (int y{ 0 }; y < Board->Height; ++y)
        {
            if (Board->Grid[x][y].isMine && !Board->Grid[x][y].isFlagged)
            {
                Board->Grid[x][y].isRevealed = true;
                UpdateTileColour(Board, VertexBuffer, x, y, hitMineX, hitMineY);
            }
        }
    }
}

int CountFlags(GameBoard* Board)
{
    int count{ 0 };
    for (int x{ 0 }; x < Board->Width; ++x)
    {
        for (int y{ 0 }; y < Board->Height; ++y)
        {
            if (Board->Grid[x][y].isFlagged)
            {
                ++count;
            }
        }
    }
    return count;
}