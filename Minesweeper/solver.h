#pragma once

#include <queue>
#include <utility>  // for std::pair
#include <vector>

#include "tile.h"
#include "board.h"
#include "game.h"

typedef void (*RevealFunction)(GameBoard*, int, int);
typedef void (*LossFunction)(GameBoard*, int x, int y);

struct GuessCandidate {
    int x;
    int y;
    float MineProbability;
    int AdjacentRevealed;
    int AdjacentUnrevealed;
    bool IsCornerOrEdge;
    float Score;
};

struct GuessResult {
    bool madeGuess;
    bool hitMine;
    int x;
    int y;
};


void SolveStep(GameBoard* Board, std::queue<std::pair<int, int>>& TilesToCheck, RevealFunction FloodFillReveal, LossFunction OnLoss);

bool Try121Pattern(GameBoard* Board, int x, int y, std::queue<std::pair<int, int>>& TilesToCheck);
bool Try12Pattern(GameBoard* Board, int x, int y, std::queue<std::pair<int, int>>& TilesToCheck);
bool TrySubsetRule(GameBoard* Board, int x, int y, std::queue<std::pair<int, int>>& TilesToCheck, RevealFunction FloodFillReveal);
GuessResult TryProbabilisticGuess(GameBoard* Board, RevealFunction FloodFillReveal, LossFunction OnLoss);