#include "render.h"
#include "geometry.h"

void RenderTexturedQuads(const std::vector<GLfloat>& Vertices,
    GLuint Texture,
    GLuint ShaderProgram,
    const glm::mat4& Projection)
{
    if (Vertices.empty()) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Upload number vertices
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * Vertices.size(), Vertices.data(), GL_DYNAMIC_DRAW);

    // Use texture shader
    glUseProgram(ShaderProgram);
    GLuint textureMatrixID = glGetUniformLocation(ShaderProgram, "MVP");
    glUniformMatrix4fv(textureMatrixID, 1, GL_FALSE, &Projection[0][0]);

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    GLuint samplerID = glGetUniformLocation(ShaderProgram, "textureSampler");
    glUniform1i(samplerID, 0);

    // Setup vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    glDrawArrays(GL_TRIANGLES, 0, Vertices.size() / 5);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glDeleteBuffers(1, &VBO);

    glDisable(GL_BLEND);
}

void BuildNumberVertices(std::vector<GLfloat>& Vertices, GameBoard* Board)
{
    for (int x{ 0 }; x < Board->Width; ++x)
    {
        for (int y{ 0 }; y < Board->Height; ++y)
        {
            if (Board->Grid[x][y].isRevealed && !Board->Grid[x][y].isMine && Board->Grid[x][y].NeighbourMines > 0)
            {
                CreateNumberQuad(Vertices, x, y, Board->Grid[x][y].NeighbourMines);
            }
        }
    }
}

void BuildFlagVertices(std::vector<GLfloat>& Vertices, GameBoard* Board)
{
    for (int x{ 0 }; x < Board->Width; ++x)
    {
        for (int y{ 0 }; y < Board->Height; ++y)
        {
            if (Board->Grid[x][y].isFlagged)
            {
                CreateFlagQuad(x, y, Vertices);
            }
        }
    }
}

void UpdateTileColour(GameBoard* Board, GLuint VertexBuffer, int gridX, int gridY, int hitMineX, int hitMineY)
{
    Tile& tile = Board->Grid[gridX][gridY];

    // Determine colour
    float r, g, b;
    if (tile.isRevealed)
    {
        if (tile.isMine)
        {
            if (gridX == hitMineX && gridY == hitMineY) {
                r = 1.0f; g = 0.0f; b = 0.0f;  // Red
            }
            // Black for other revealed mines
            else {
                r = 0.0f; g = 0.0f; b = 0.0f;  // Black
            }
        }
        else
        {
            r = 0.8f; g = 0.8f; b = 0.8f;  // Light grey
        }
    }
    else
    {
        r = 0.5f; g = 0.5f; b = 0.5f;  // Dark grey
    }

    // Calculate buffer offset
    int tileIndex = gridX * Board->Height + gridY;
    int floatsPerTile = 36;  // 6 vertices * 6 floats
    int baseOffset = tileIndex * floatsPerTile;

    // Update colour for all 6 vertices of this tile
    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
    for (int vertex = 0; vertex < 6; ++vertex)
    {
        int colourOffset = baseOffset + (vertex * 6) + 3;  // Skip x,y,z to get to r,g,b
        float colours[3] = { r, g, b };
        glBufferSubData(GL_ARRAY_BUFFER,
            colourOffset * sizeof(GLfloat),
            3 * sizeof(GLfloat),
            colours);
    }
}

std::vector<GLfloat> BuildGridVertices(int Width, int Height, float Gap)
{
    std::vector<GLfloat> GridVertices;
    for (int x{ 0 }; x < Width; ++x)
    {
        for (int y{ 0 }; y < Height; ++y)
        {
            // per vertex: x, y, z, r, g, b
            GridVertices.push_back(static_cast<float>(x));
            GridVertices.push_back(static_cast<float>(y));
            GridVertices.push_back(0.0f);
            GridVertices.push_back(0.5f);
            GridVertices.push_back(0.5f);
            GridVertices.push_back(0.5f);

            GridVertices.push_back(static_cast<float>(x + 1 - Gap));
            GridVertices.push_back(static_cast<float>(y));
            GridVertices.push_back(0.0f);
            GridVertices.push_back(0.5f);
            GridVertices.push_back(0.5f);
            GridVertices.push_back(0.5f);

            GridVertices.push_back(static_cast<float>(x + 1 - Gap));
            GridVertices.push_back(static_cast<float>(y + 1 - Gap));
            GridVertices.push_back(0.0f);
            GridVertices.push_back(0.5f);
            GridVertices.push_back(0.5f);
            GridVertices.push_back(0.5f);

            GridVertices.push_back(static_cast<float>(x));
            GridVertices.push_back(static_cast<float>(y));
            GridVertices.push_back(0.0f);
            GridVertices.push_back(0.5f);
            GridVertices.push_back(0.5f);
            GridVertices.push_back(0.5f);

            GridVertices.push_back(static_cast<float>(x));
            GridVertices.push_back(static_cast<float>(y + 1 - Gap));
            GridVertices.push_back(0.0f);
            GridVertices.push_back(0.5f);
            GridVertices.push_back(0.5f);
            GridVertices.push_back(0.5f);

            GridVertices.push_back(static_cast<float>(x + 1 - Gap));
            GridVertices.push_back(static_cast<float>(y + 1 - Gap));
            GridVertices.push_back(0.0f);
            GridVertices.push_back(0.5f);
            GridVertices.push_back(0.5f);
            GridVertices.push_back(0.5f);
        }
    }
    return GridVertices;
}

void BuildThreeDigitDisplay(std::vector<GLfloat>& Vertices, int value, float startX, float y, float digitSpacing)
{
    int hundreds = value / 100;
    int tens = (value / 10) % 10;
    int ones = value % 10;

    CreateDigitQuad(Vertices, startX, y, hundreds, 2);
    CreateDigitQuad(Vertices, startX + digitSpacing, y, tens, 2);
    CreateDigitQuad(Vertices, startX + digitSpacing * 2, y, ones, 2);
}