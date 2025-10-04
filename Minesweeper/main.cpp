#include <stdio.h>
#include <stdlib.h>
#include <direct.h> // cwd

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <array>
#include <queue>
#include <random>

#include "game.h"
#include "shader.h"
#include "solver.h"
#include "geometry.h"
#include "render.h"
#include "texture.h"
#include "tile.h"

static std::random_device rd;
static std::mt19937 gen(rd());

using namespace glm;

enum GameState {
    PLAYING,
    WON,
    LOST
};

GameState CurrentGameState = PLAYING;

GLuint programID;
GLuint VertexBuffer;
GLuint TextureShaderID;
GLuint NumberTexture;
GLuint FlagTexture;
GLuint DigitTexture;

int ScreenWidth = 1920;
int ScreenHeight = 1080;

const int GridWidth = 30;
const int GridHeight = 16;
std::array<std::array<Tile, GridHeight>, GridWidth> Grid;
const int MineCount = 99;
bool FirstClick = true;
int HitMineX = -1;
int HitMineY = -1;
double LossTime = 0.0;

double GameTime = 0.0;
double PreviousGameTime = 0.0;
bool TimerRunning = false;

std::queue<std::pair<int, int>> TilesToCheck;
static double lastSolveTime = 0.0;
static bool AutoSolverEnabled = false;

static long WinCount = 0;
static long PlayCount = 0;

void FloodFillReveal(GameBoard* Board, int x, int y) // DFS RECURSION
{
    if (x < 0 || x >= GridWidth || y < 0 || y >= GridHeight)
        return;  // Out of bounds

    if (Grid[x][y].isRevealed || Grid[x][y].isMine)
        return;  // Already revealed or is a mine

    // Reveal this tile
    Grid[x][y].isRevealed = true;
    UpdateTileColour(Board, VertexBuffer, x, y);

    // If this tile has neighbouring mines, stop here
    if (Grid[x][y].NeighbourMines > 0)
        return;

    // Otherwise, recurse to all 8 neighbours
    for (int offsetX = -1; offsetX <= 1; ++offsetX)
    {
        for (int offsetY = -1; offsetY <= 1; ++offsetY)
        {
            if (offsetX == 0 && offsetY == 0)
                continue;  // Skip self

            FloodFillReveal(Board, x + offsetX, y + offsetY);
        }
    }
}

void ResetGame(GameBoard* Board)
{
    ++PlayCount;
    if (CurrentGameState == WON)
    {
        ++WinCount;
        printf("Game %d ended as WIN, %d games won total\n", PlayCount, WinCount);
    }
    else
    {
        printf("Game %d ended as LOSS, %d games won total\n", PlayCount, WinCount);
    }
    ClearGrid(Board);
    FirstClick = true;
    CurrentGameState = PLAYING;
    GameTime = 0.0;
    TimerRunning = false;

    while (!TilesToCheck.empty()) TilesToCheck.pop();

    // Regenerate tile colours (back to unrevealed grey)
    for (int x = 0; x < GridWidth; ++x)
    {
        for (int y = 0; y < GridHeight; ++y)
        {
            UpdateTileColour(Board, VertexBuffer, x, y);
        }
    }

    if (AutoSolverEnabled)
    {
        std::uniform_int_distribution<> distrX(0, 29);
        std::uniform_int_distribution<> distrY(0, 15);

        int startX{ distrX(gen) };
        int startY{ distrY(gen) };

        PopulateMines(Board, startX, startY);  // Generate mines avoiding start position
        SetNeighbourCounts(Board);           // Calculate neighbour counts
        TimerRunning = true;
        FirstClick = false;             // Important: mines are now placed

        FloodFillReveal(Board, startX, startY);
    }
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    mat4 projection = ortho(0.0f, 30.0f, 0.0f, 18.0f, -1.0f, 1.0f);
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &projection[0][0]);

    glfwGetWindowSize(window, &ScreenWidth, &ScreenHeight);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (action != GLFW_PRESS) return;
    if (CurrentGameState != PLAYING) return;

    GameBoard* Board { (GameBoard*)glfwGetWindowUserPointer(window) };

    double cursorX, cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);

    double orthoX = (cursorX / ScreenWidth) * 30.0f;
    double orthoY = 18.0f - (cursorY / ScreenHeight) * 18.0f;

    int gridX = (int)orthoX;
    int gridY = (int)orthoY;

    switch (button)
    {
        case GLFW_MOUSE_BUTTON_LEFT:
        {
            if (gridX < 0 || gridX >= GridWidth || gridY < 0 || gridY >= GridHeight) return;
            if (Grid[gridX][gridY].isFlagged) return;
            if (FirstClick)
            {
                PopulateMines(Board, gridX, gridY);
                SetNeighbourCounts(Board);
                TimerRunning = true;
                GameTime = 0.0;

                for (int x{ 0 }; x < GridWidth; ++x)
                {
                    for (int y{ 0 }; y < GridHeight; ++y)
                    {
                        if (Grid[x][y].isRevealed && Grid[x][y].NeighbourMines > 0)
                        {
                            TilesToCheck.push({ x, y });
                        }
                    }
                }
            }
            if (Grid[gridX][gridY].isMine)
            {
                Grid[gridX][gridY].isRevealed = true;
                HitMineX = gridX;  // Store which mine was clicked
                HitMineY = gridY;
                UpdateTileColour(Board, VertexBuffer, gridX, gridY, gridX, gridY);
                CurrentGameState = LOST;
                TimerRunning = false;
                LossTime = glfwGetTime();  // Record loss time
                RevealAllMines(Board, VertexBuffer, gridX, gridY);
            }
            else
            {
                FloodFillReveal(Board, gridX, gridY);
                if (CheckWinCondition(Board))
                {
                    CurrentGameState = WON;
                    TimerRunning = false;
                }
            }
            FirstClick = false;
            return;
        }
        case GLFW_MOUSE_BUTTON_RIGHT:
        {
            if (Grid[gridX][gridY].isRevealed) return;
            Grid[gridX][gridY].isFlagged = !Grid[gridX][gridY].isFlagged;
            break;
        }
    }
}

void HandleLoss(GameBoard* Board, int x, int y)
{
    HitMineX = x;
    HitMineY = y;
    UpdateTileColour(Board, VertexBuffer, x, y, x, y);
    CurrentGameState = LOST;
    TimerRunning = false;
    LossTime = glfwGetTime();
    RevealAllMines(Board, VertexBuffer, x, y);
}

int main() {
    // Start init
    if (!glfwInit())
    {
        printf("Failed to init GLFW");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    window = glfwCreateWindow(ScreenWidth, ScreenHeight, "Minesweeper", NULL, NULL);

    if (window == NULL)
    {
        printf("Failed to create window");
        glfwTerminate();
        return -1;
    }

    glfwSetWindowAspectRatio(window, 30, 18);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialise GLAD");
        return -1;
    }

    glfwSetWindowSizeCallback(window, FramebufferSizeCallback);
    glfwGetWindowSize(window, &ScreenWidth, &ScreenHeight);
    FramebufferSizeCallback(window, ScreenWidth, ScreenHeight);

    glfwSetMouseButtonCallback(window, MouseButtonCallback);

    GLFWimage images[1];
    images[0] = LoadImage(".\\textures\\flag.png");
    glfwSetWindowIcon(window, 1, images);

    NumberTexture = LoadTexture(".\\textures\\numberstransparent.png");
    FlagTexture = LoadTexture(".\\textures\\flag.png");
    DigitTexture = LoadTexture(".\\textures\\digits.png");
    // End init

    /*
     * char cwd[1024];
     * getcwd(cwd, sizeof(cwd));
     * printf("Current directory: %s\n", cwd);
    */

    programID = LoadShaders(".\\shaders\\VertexShader.vert", 
                            ".\\shaders\\FragmentShader.frag");

    TextureShaderID = LoadShaders(".\\shaders\\TextureVertexShader.vert",
                                  ".\\shaders\\TextureFragmentShader.frag");

    mat4 projection = ortho(0.0f, 30.0f, 0.0f, 18.0f, -1.0f, 1.0f);

    std::vector<GLfloat> GridVertices{ BuildGridVertices(GridWidth, GridHeight, 0.02f) };

    // VAOs and VBOs
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // GLuint VertexBuffer;
    glGenBuffers(1, &VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * GridVertices.size(), GridVertices.data(), GL_STATIC_DRAW);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);  // Light grey background

    Tile* GridPointers[GridWidth];
    for (int i = 0; i < GridWidth; ++i) {
        GridPointers[i] = Grid[i].data();
    }

    GameBoard Board;
    Board.Grid = GridPointers;
    Board.Height = GridHeight;
    Board.Width = GridWidth;
    Board.MineCount = MineCount;

    glfwSetWindowUserPointer(window, &Board);
    // main loop
    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programID);
        GLuint MatrixID = glGetUniformLocation(programID, "MVP");
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &projection[0][0]);

        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)0);
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
        glDrawArrays(GL_TRIANGLES, 0, GridVertices.size() / 3);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);    

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            ResetGame(&Board);

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            AutoSolverEnabled = !AutoSolverEnabled;

        if (AutoSolverEnabled && CurrentGameState == PLAYING)
        {
            double currentTime = glfwGetTime();
            if (currentTime - lastSolveTime >= 0.0f)  // Timer between moves
            {
                SolveStep(&Board, TilesToCheck, FloodFillReveal, HandleLoss);
                lastSolveTime = currentTime;
            }
        }

        // Draw tile numbers
        std::vector<GLfloat> NumberVertices;
        BuildNumberVertices(NumberVertices, &Board);
        RenderTexturedQuads(NumberVertices, NumberTexture, TextureShaderID, projection);

        // Draw flags
        std::vector<GLfloat> FlagVertices;
        BuildFlagVertices(FlagVertices, &Board);
        RenderTexturedQuads(FlagVertices, FlagTexture, TextureShaderID, projection);

        // Draw mine count, time
        std::vector<GLfloat> DigitVertices;
        int MinesRemaining = MineCount - CountFlags(&Board);
        BuildThreeDigitDisplay(DigitVertices, MinesRemaining, 0.0f, 16.0f, 1.2f);
        int Seconds = (int)GameTime;
        BuildThreeDigitDisplay(DigitVertices, Seconds, 25.7f, 16.0f, 1.2f);

        RenderTexturedQuads(DigitVertices, DigitTexture, TextureShaderID, projection);

        // End frame stuff
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Timer
        double CurrentTime{ glfwGetTime() };
        double DeltaTime{ CurrentTime - PreviousGameTime };
        if (TimerRunning)
            GameTime += DeltaTime;
        PreviousGameTime = CurrentTime;

        if (CheckWinCondition(&Board))
        {
            CurrentGameState = WON;
            TimerRunning = false;
            FirstClick = true;
        }

        if ((CurrentGameState == WON || CurrentGameState == LOST) && AutoSolverEnabled)
            if (glfwGetTime() - LossTime >= 1.0)  // 1 second delay
                ResetGame(&Board);
    } while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS);
    return 0;
}