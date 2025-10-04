#include "solver.h"

void QueueForCheck(GameBoard* Board, int x, int y, std::queue<std::pair<int, int>>& TilesToCheck)
{
    for (int offsetX{ -1 }; offsetX <= 1; ++offsetX)
    {
        for (int offsetY{ -1 }; offsetY <= 1; ++offsetY)
        {
            if (offsetX == 0 && offsetY == 0)
            {
                continue;
            }

            int neighbourX{ x + offsetX };
            int neighbourY{ y + offsetY };

            // Check bounds
            if (neighbourX >= 0 && neighbourX < Board->Width &&
                neighbourY >= 0 && neighbourY < Board->Height)
            {
                if (Board->Grid[neighbourX][neighbourY].isRevealed && 
                    Board->Grid[neighbourX][neighbourY].NeighbourMines > 0)
                {
                    TilesToCheck.push({ neighbourX, neighbourY });
                }
            }
        }
    }
}

bool Try121Pattern(GameBoard* Board, int x, int y, std::queue<std::pair<int, int>>& TilesToCheck)
{
    if (!Board->Grid[x][y].isRevealed || Board->Grid[x][y].NeighbourMines != 2)
        return false;

    // Check horizontal 1-2-1
    if (x >= 1 && x < Board->Width - 1)
    {
        if (Board->Grid[x - 1][y].isRevealed && Board->Grid[x - 1][y].NeighbourMines == 1 &&
            Board->Grid[x + 1][y].isRevealed && Board->Grid[x + 1][y].NeighbourMines == 1)
        {
            // Verify pattern constraints:
            // - Each "1" must have exactly 1 unrevealed neighbour
            // - Those must be the perpendicular cells to the "2"

            int UnrevealedLeft{ 0 };
            int UnrevealedRight{ 0 };
            int UnrevealedMiddle{ 0 };

            // Count unrevealed for left "1"
            for (int oy{ -1 }; oy <= 1; ++oy)
            {
                for (int ox{ -1 }; ox <= 1; ++ox)
                {
                    if (ox == 0 && oy == 0) continue;
                    int nx = x - 1 + ox;
                    int ny = y + oy;
                    if (nx >= 0 && nx < Board->Width && ny >= 0 && ny < Board->Height)
                        if (!Board->Grid[nx][ny].isRevealed && !Board->Grid[nx][ny].isFlagged)
                            ++UnrevealedLeft;
                }
            }

            // Count unrevealed for right "1"
            for (int oy{ -1 }; oy <= 1; ++oy)
            {
                for (int ox{ -1 }; ox <= 1; ++ox)
                {
                    if (ox == 0 && oy == 0) continue;
                    int nx = x + 1 + ox;
                    int ny = y + oy;
                    if (nx >= 0 && nx < Board->Width && ny >= 0 && ny < Board->Height)
                        if (!Board->Grid[nx][ny].isRevealed && !Board->Grid[nx][ny].isFlagged)
                            ++UnrevealedRight;
                }
            }

            // Count unrevealed for middle "2"
            for (int oy{ -1 }; oy <= 1; ++oy)
            {
                for (int ox{ -1 }; ox <= 1; ++ox)
                {
                    if (ox == 0 && oy == 0) continue;
                    int nx = x + ox;
                    int ny = y + oy;
                    if (nx >= 0 && nx < Board->Width && ny >= 0 && ny < Board->Height)
                        if (!Board->Grid[nx][ny].isRevealed && !Board->Grid[nx][ny].isFlagged)
                            ++UnrevealedMiddle;
                }
            }

            // Pattern only applies if each "1" has exactly 1 unrevealed
            // and the "2" has exactly 2 unrevealed (the perpendicular cells)
            if (UnrevealedLeft == 1 && UnrevealedRight == 1 && UnrevealedMiddle == 2)
            {
                // Flag perpendicular cells
                if (y > 0 && !Board->Grid[x][y - 1].isRevealed && !Board->Grid[x][y - 1].isFlagged)
                {
                    Board->Grid[x][y - 1].isFlagged = true;
                    QueueForCheck(Board, x, y - 1, TilesToCheck);
                    return true;
                }
                if (y < Board->Height - 1 && !Board->Grid[x][y + 1].isRevealed && !Board->Grid[x][y + 1].isFlagged)
                {
                    Board->Grid[x][y + 1].isFlagged = true;
                    QueueForCheck(Board, x, y + 1, TilesToCheck);
                    return true;
                }
            }
        }
    }

    // Check vertical 1-2-1 (similar logic)
    if (y >= 1 && y < Board->Height - 1)
    {
        if (Board->Grid[x][y - 1].isRevealed && Board->Grid[x][y - 1].NeighbourMines == 1 &&
            Board->Grid[x][y + 1].isRevealed && Board->Grid[x][y + 1].NeighbourMines == 1)
        {
            int UnrevealedTop{ 0 };
            int UnrevealedBottom{ 0 };
            int UnrevealedMiddle{ 0 };

            for (int ox{ -1 }; ox <= 1; ++ox)
            {
                for (int oy{ -1 }; oy <= 1; ++oy)
                {
                    if (ox == 0 && oy == 0) continue;
                    int nx = x + ox;
                    int ny = y - 1 + oy;
                    if (nx >= 0 && nx < Board->Width && ny >= 0 && ny < Board->Height)
                        if (!Board->Grid[nx][ny].isRevealed && !Board->Grid[nx][ny].isFlagged)
                            ++UnrevealedTop;
                }
            }

            for (int ox{ -1 }; ox <= 1; ++ox)
            {
                for (int oy{ -1 }; oy <= 1; ++oy)
                {
                    if (ox == 0 && oy == 0) continue;
                    int nx = x + ox;
                    int ny = y + 1 + oy;
                    if (nx >= 0 && nx < Board->Width && ny >= 0 && ny < Board->Height)
                        if (!Board->Grid[nx][ny].isRevealed && !Board->Grid[nx][ny].isFlagged)
                            ++UnrevealedBottom;
                }
            }

            for (int ox{ -1 }; ox <= 1; ++ox)
            {
                for (int oy{ -1 }; oy <= 1; ++oy)
                {
                    if (ox == 0 && oy == 0) continue;
                    int nx = x + ox;
                    int ny = y + oy;
                    if (nx >= 0 && nx < Board->Width && ny >= 0 && ny < Board->Height)
                        if (!Board->Grid[nx][ny].isRevealed && !Board->Grid[nx][ny].isFlagged)
                            ++UnrevealedMiddle;
                }
            }

            if (UnrevealedTop == 1 && UnrevealedBottom == 1 && UnrevealedMiddle == 2)
            {
                if (x > 0 && !Board->Grid[x - 1][y].isRevealed && !Board->Grid[x - 1][y].isFlagged)
                {
                    Board->Grid[x - 1][y].isFlagged = true;
                    QueueForCheck(Board, x - 1, y, TilesToCheck);
                    return true;
                }
                if (x < Board->Width - 1 && !Board->Grid[x + 1][y].isRevealed && !Board->Grid[x + 1][y].isFlagged)
                {
                    Board->Grid[x + 1][y].isFlagged = true;
                    QueueForCheck(Board, x + 1, y, TilesToCheck);
                    return true;
                }
            }
        }
    }
    return false;
}

bool Try12Pattern(GameBoard* Board, int x, int y, std::queue<std::pair<int, int>>& TilesToCheck)
{
    if (!Board->Grid[x][y].isRevealed || Board->Grid[x][y].NeighbourMines != 1)
        return false;

    // Check all neighbours for a "2"
    for (int offsetX{ -1 }; offsetX <= 1; ++offsetX)
    {
        for (int offsetY{ -1 }; offsetY <= 1; ++offsetY)
        {
            if (offsetX == 0 && offsetY == 0) continue;

            int nx{ x + offsetX };
            int ny{ y + offsetY };

            if (nx < 0 || nx >= Board->Width || ny < 0 || ny >= Board->Height)
                continue;

            if (Board->Grid[nx][ny].isRevealed && Board->Grid[nx][ny].NeighbourMines == 2)
            {
                // Get unrevealed neighbours of the "1"
                std::vector<std::pair<int, int>> Unrevealed1;
                for (int ox{ -1 }; ox <= 1; ++ox)
                {
                    for (int oy{ -1 }; oy <= 1; ++oy)
                    {
                        if (ox == 0 && oy == 0) continue;
                        int ux = x + ox;
                        int uy = y + oy;
                        if (ux >= 0 && ux < Board->Width && uy >= 0 && uy < Board->Height)
                        {
                            if (!Board->Grid[ux][uy].isRevealed)
                                Unrevealed1.push_back({ ux, uy });
                        }
                    }
                }

                // If "1" has exactly one unrevealed neighbour, and it's shared with "2"
                if (Unrevealed1.size() == 1)
                {
                    int sharedX = Unrevealed1[0].first;
                    int sharedY = Unrevealed1[0].second;

                    // Check if this is a neighbour of the "2" as well
                    bool isShared = false;
                    if (abs(sharedX - nx) <= 1 && abs(sharedY - ny) <= 1)
                        isShared = true;

                    if (isShared)
                    {
                        // Flag the shared cell
                        if (!Board->Grid[sharedX][sharedY].isFlagged)
                        {
                            Board->Grid[sharedX][sharedY].isFlagged = true;
                            QueueForCheck(Board, sharedX, sharedY, TilesToCheck);
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool TrySubsetRule(GameBoard* Board, int x, int y, std::queue<std::pair<int, int>>& TilesToCheck, RevealFunction FloodFillReveal)
{
    if (!Board->Grid[x][y].isRevealed || Board->Grid[x][y].NeighbourMines == 0)
        return false;

    // Get unrevealed neighbours of this tile (set A)
    std::vector<std::pair<int, int>> UnrevealedA;
    for (int offsetX{ -1 }; offsetX <= 1; ++offsetX)
    {
        for (int offsetY{ -1 }; offsetY <= 1; ++offsetY)
        {
            if (offsetX == 0 && offsetY == 0) continue;
            int nx{ x + offsetX };
            int ny{ y + offsetY };
            if (nx >= 0 && nx < Board->Width && ny >= 0 && ny < Board->Height)
            {
                if (!Board->Grid[nx][ny].isRevealed)
                    UnrevealedA.push_back({ nx, ny });
            }
        }
    }

    if (UnrevealedA.empty()) return false;

    // Compare against all other revealed numbered tiles
    for (int bx{ 0 }; bx < Board->Width; ++bx)
    {
        for (int by{ 0 }; by < Board->Height; ++by)
        {
            if (bx == x && by == y) continue;
            if (!Board->Grid[bx][by].isRevealed || Board->Grid[bx][by].NeighbourMines == 0)
                continue;

            // Only check nearby tiles (within 2 squares for efficiency)
            if (abs(bx - x) > 2 || abs(by - y) > 2)
                continue;

            // Get unrevealed neighbours of tile B
            std::vector<std::pair<int, int>> UnrevealedB;
            for (int offsetX{ -1 }; offsetX <= 1; ++offsetX)
            {
                for (int offsetY{ -1 }; offsetY <= 1; ++offsetY)
                {
                    if (offsetX == 0 && offsetY == 0) continue;
                    int nx{ bx + offsetX };
                    int ny{ by + offsetY };
                    if (nx >= 0 && nx < Board->Width && ny >= 0 && ny < Board->Height)
                    {
                        if (!Board->Grid[nx][ny].isRevealed)
                            UnrevealedB.push_back({ nx, ny });
                    }
                }
            }

            if (UnrevealedB.empty()) continue;

            // Check if A is subset of B
            bool AisSubsetOfB = true;
            for (const auto& cellA : UnrevealedA)
            {
                bool found = false;
                for (const auto& cellB : UnrevealedB)
                {
                    if (cellA.first == cellB.first && cellA.second == cellB.second)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    AisSubsetOfB = false;
                    break;
                }
            }

            if (AisSubsetOfB && UnrevealedA.size() < UnrevealedB.size())
            {
                int minesA = Board->Grid[x][y].NeighbourMines;
                int minesB = Board->Grid[bx][by].NeighbourMines;

                // Count flagged neighbours for both
                int flaggedA = 0;
                int flaggedB = 0;
                for (const auto& cell : UnrevealedA)
                    if (Board->Grid[cell.first][cell.second].isFlagged) ++flaggedA;
                for (const auto& cell : UnrevealedB)
                    if (Board->Grid[cell.first][cell.second].isFlagged) ++flaggedB;

                int remainingMinesA = minesA - flaggedA;
                int remainingMinesB = minesB - flaggedB;

                // If A and B have same remaining mines, B's extras are safe
                if (remainingMinesA == remainingMinesB)
                {
                    for (const auto& cellB : UnrevealedB)
                    {
                        bool inA = false;
                        for (const auto& cellA : UnrevealedA)
                        {
                            if (cellA.first == cellB.first && cellA.second == cellB.second)
                            {
                                inA = true;
                                break;
                            }
                        }
                        if (!inA && !Board->Grid[cellB.first][cellB.second].isFlagged)
                        {
                            FloodFillReveal(Board, cellB.first, cellB.second);
                            QueueForCheck(Board, cellB.first, cellB.second, TilesToCheck);
                            return true;
                        }
                    }
                }

                // If difference equals extra cells, those extras are all mines
                int extraCells = UnrevealedB.size() - UnrevealedA.size();
                if (remainingMinesB - remainingMinesA == extraCells)
                {
                    for (const auto& cellB : UnrevealedB)
                    {
                        bool inA = false;
                        for (const auto& cellA : UnrevealedA)
                        {
                            if (cellA.first == cellB.first && cellA.second == cellB.second)
                            {
                                inA = true;
                                break;
                            }
                        }
                        if (!inA && !Board->Grid[cellB.first][cellB.second].isFlagged)
                        {
                            Board->Grid[cellB.first][cellB.second].isFlagged = true;
                            QueueForCheck(Board, cellB.first, cellB.second, TilesToCheck);
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

GuessResult TryProbabilisticGuess(GameBoard* Board, RevealFunction FloodFillReveal, LossFunction OnLoss)
{
    std::vector<GuessCandidate> Candidates;
    GuessResult result { false, false, -1, -1 };

    // Collect all unrevealed cells with their stats
    for (int x{ 0 }; x < Board->Width; ++x)
    {
        for (int y{ 0 }; y < Board->Height; ++y)
        {
            if (Board->Grid[x][y].isRevealed || Board->Grid[x][y].isFlagged)
                continue;

            GuessCandidate Candidate;
            Candidate.x = x;
            Candidate.y = y;

            // Calculate mine probability
            std::vector<float> Probabilities;
            int AdjacentRevealed{ 0 };
            int AdjacentUnrevealed{ 0 };

            for (int offsetX{ -1 }; offsetX <= 1; ++offsetX)
            {
                for (int offsetY{ -1 }; offsetY <= 1; ++offsetY)
                {
                    if (offsetX == 0 && offsetY == 0) continue;

                    int nx{ x + offsetX };
                    int ny{ y + offsetY };

                    if (nx < 0 || nx >= Board->Width || ny < 0 || ny >= Board->Height)
                        continue;

                    if (Board->Grid[nx][ny].isRevealed)
                    {
                        ++AdjacentRevealed;

                        if (Board->Grid[nx][ny].NeighbourMines > 0)
                        {
                            // Count this revealed tile's stats
                            int Unrevealed{ 0 };
                            int Flagged{ 0 };

                            for (int ox{ -1 }; ox <= 1; ++ox)
                            {
                                for (int oy{ -1 }; oy <= 1; ++oy)
                                {
                                    if (ox == 0 && oy == 0) continue;
                                    int nnx{ nx + ox };
                                    int nny{ ny + oy };
                                    if (nnx >= 0 && nnx < Board->Width && nny >= 0 && nny < Board->Height)
                                    {
                                        if (!Board->Grid[nnx][nny].isRevealed && !Board->Grid[nnx][nny].isFlagged)
                                            ++Unrevealed;
                                        if (Board->Grid[nnx][nny].isFlagged)
                                            ++Flagged;
                                    }
                                }
                            }

                            if (Unrevealed > 0)
                            {
                                int RemainingMines{ Board->Grid[nx][ny].NeighbourMines - Flagged };
                                float Probability{ (float)RemainingMines / (float)Unrevealed };
                                Probabilities.push_back(Probability);
                            }
                        }
                    }
                    else if (!Board->Grid[nx][ny].isFlagged)
                    {
                        ++AdjacentUnrevealed;
                    }
                }
            }

            // If no local probability info, use global
            if (Probabilities.empty())
            {
                int TotalUnrevealed{ 0 };
                int TotalFlags{ CountFlags(Board)};

                for (int gx{ 0 }; gx < Board->Width; ++gx)
                {
                    for (int gy{ 0 }; gy < Board->Height; ++gy)
                    {
                        if (!Board->Grid[gx][gy].isRevealed && !Board->Grid[gx][gy].isFlagged)
                            ++TotalUnrevealed;
                    }
                }

                if (TotalUnrevealed > 0)
                {
                    int RemainingMines{ Board->MineCount - TotalFlags };
                    float GlobalProb{ (float)RemainingMines / (float)TotalUnrevealed };
                    Probabilities.push_back(GlobalProb);
                }
            }

            // Average probabilities
            float AvgProbability{ 0.0f };
            if (!Probabilities.empty())
            {
                for (float p : Probabilities)
                    AvgProbability += p;
                AvgProbability /= Probabilities.size();
            }

            Candidate.MineProbability = AvgProbability;
            Candidate.AdjacentRevealed = AdjacentRevealed;
            Candidate.AdjacentUnrevealed = AdjacentUnrevealed;

            // Check if corner or edge
            int NeighbourCount{ 0 };
            for (int ox{ -1 }; ox <= 1; ++ox)
            {
                for (int oy{ -1 }; oy <= 1; ++oy)
                {
                    if (ox == 0 && oy == 0) continue;
                    int nx{ x + ox };
                    int ny{ y + oy };
                    if (nx >= 0 && nx < Board->Width && ny >= 0 && ny < Board->Height)
                        ++NeighbourCount;
                }
            }
            Candidate.IsCornerOrEdge = (NeighbourCount < 8);

            // Calculate composite score (lower is better)
            // Weights tuned for good guessing behavior
            Candidate.Score = Candidate.MineProbability * 100.0f;  // Primary factor
            Candidate.Score -= Candidate.AdjacentRevealed * 2.0f;  // More info = better
            Candidate.Score += Candidate.AdjacentUnrevealed * 0.5f; // Fewer unrevealed = safer
            Candidate.Score -= Candidate.IsCornerOrEdge ? 5.0f : 0.0f; // Edges preferred

            Candidates.push_back(Candidate);
        }
    }

    if (Candidates.empty())
        return result;

    // Sort by score (lower is better)
    std::sort(Candidates.begin(), Candidates.end(),
        [](const GuessCandidate& a, const GuessCandidate& b) {
            return a.Score < b.Score;
        });

    // Pick the best candidate
    GuessCandidate Best = Candidates[0];

    result.madeGuess = true;
    result.x = Best.x;
    result.y = Best.y;

    printf("Guessing at (%d, %d): Prob=%.1f%%, AdjRev=%d, AdjUnrev=%d, Edge=%d, Score=%.1f\n",
        Best.x, Best.y, Best.MineProbability * 100.0f,
        Best.AdjacentRevealed, Best.AdjacentUnrevealed,
        Best.IsCornerOrEdge, Best.Score);

    FloodFillReveal(Board, Best.x, Best.y);

    if (Board->Grid[Best.x][Best.y].isMine)
    {
        Board->Grid[Best.x][Best.y].isRevealed = true;
        result.hitMine = true;
        OnLoss(Board, Best.x, Best.y);
    }

    return result;
}

void SolveStep(GameBoard* Board, std::queue<std::pair<int, int>>& TilesToCheck, RevealFunction FloodFillReveal, LossFunction OnLoss)
{
    /*
    for (int i{ 0 }; i < TilesToCheck.size(); ++i)
    {
        printf("Queue %l: (%l, %l)\n", i, &(TilesToCheck.front() + i).first, &(TilesToCheck.front() + i).second);
    }
    */
    static int count{ 0 };
    /*
    if (!TilesToCheck.empty())
        printf("Next check %d: (%d, %d)\n", count, TilesToCheck.front().first, TilesToCheck.front().second); ++count;
    */
    if (TilesToCheck.empty())
    {
        printf("Queue empty, rescanning grid\n");
        // Queue empty - do a full grid scan to find new starting points
        for (int x{ 0 }; x < Board->Width; ++x)
        {
            for (int y{ 0 }; y < Board->Height; ++y)
            {
                if (Board->Grid[x][y].isRevealed && Board->Grid[x][y].NeighbourMines > 0)
                {
                    TilesToCheck.push({ x, y });
                }
            }
        }
        if (TilesToCheck.empty()) return;  // Nothing to solve
    }

    auto Point = TilesToCheck.front();
    TilesToCheck.pop();
    int x{ Point.first };
    int y{ Point.second };

    // printf("Checking tile (%d, %d) - Number: %d\n", x, y, Grid[x][y].NeighbourMines);

    if (!Board->Grid[x][y].isRevealed || Board->Grid[x][y].NeighbourMines == 0)
        return;

    int Unrevealed{ 0 };
    int Flagged{ 0 };

    // Count neighbours
    for (int offsetX{ -1 }; offsetX <= 1; ++offsetX)
    {
        for (int offsetY{ -1 }; offsetY <= 1; ++offsetY)
        {
            if (offsetX == 0 && offsetY == 0) continue;

            int neighbourX{ x + offsetX };
            int neighbourY{ y + offsetY };

            if (neighbourX >= 0 && neighbourX < Board->Width &&
                neighbourY >= 0 && neighbourY < Board->Height)
            {
                if (!Board->Grid[neighbourX][neighbourY].isRevealed && !Board->Grid[neighbourX][neighbourY].isFlagged)
                    ++Unrevealed;
                if (Board->Grid[neighbourX][neighbourY].isFlagged)
                    ++Flagged;
            }
        }
    }

    // Rule 1: All remaining unrevealed neighbours are mines
    if (Unrevealed == (Board->Grid[x][y].NeighbourMines - Flagged))
    {
        // printf("  Rule 1 triggered! Unrevealed=%d, Mines-Flagged=%d\n", Unrevealed, Grid[x][y].NeighbourMines - Flagged);
        for (int offsetX{ -1 }; offsetX <= 1; ++offsetX)
        {
            for (int offsetY{ -1 }; offsetY <= 1; ++offsetY)
            {
                if (offsetX == 0 && offsetY == 0) continue;

                int neighbourX{ x + offsetX };
                int neighbourY{ y + offsetY };

                if (neighbourX >= 0 && neighbourX < Board->Width &&
                    neighbourY >= 0 && neighbourY < Board->Height)
                {
                    if (!Board->Grid[neighbourX][neighbourY].isRevealed &&
                        !Board->Grid[neighbourX][neighbourY].isFlagged)
                    {
                        Board->Grid[neighbourX][neighbourY].isFlagged = true;
                        QueueForCheck(Board, neighbourX, neighbourY, TilesToCheck);
                        return;
                    }
                }
            }
        }
    }

    // Rule 2: All mines flagged, remaining neighbours are safe
    if (Flagged == Board->Grid[x][y].NeighbourMines)
    {
        // printf("  Rule 2 triggered! Unrevealed=%d, Mines-Flagged=%d\n", Unrevealed, Grid[x][y].NeighbourMines - Flagged);
        for (int offsetX{ -1 }; offsetX <= 1; ++offsetX)
        {
            for (int offsetY{ -1 }; offsetY <= 1; ++offsetY)
            {
                if (offsetX == 0 && offsetY == 0) continue;

                int neighbourX{ x + offsetX };
                int neighbourY{ y + offsetY };

                if (neighbourX >= 0 && neighbourX < Board->Width &&
                    neighbourY >= 0 && neighbourY < Board->Height)
                {
                    if (!Board->Grid[neighbourX][neighbourY].isRevealed &&
                        !Board->Grid[neighbourX][neighbourY].isFlagged)
                    {
                        FloodFillReveal(Board, neighbourX, neighbourY);
                        QueueForCheck(Board, neighbourX, neighbourY, TilesToCheck);
                        return;
                    }
                }
            }
        }
    }

    if (Try121Pattern(Board, x, y, TilesToCheck)) { return; }
    if (Try12Pattern(Board, x, y, TilesToCheck)) { return; }
    if (TrySubsetRule(Board, x, y, TilesToCheck, FloodFillReveal)) { return; }
    if (TilesToCheck.empty())
    {
        if (TryProbabilisticGuess(Board, FloodFillReveal, OnLoss).madeGuess)
            return;
    }
}