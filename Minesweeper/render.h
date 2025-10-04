#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "board.h"

void RenderTexturedQuads(const std::vector<GLfloat>& Vertices,
    GLuint Texture,
    GLuint ShaderProgram,
    const glm::mat4& Projection);

void BuildNumberVertices(std::vector<GLfloat>& Vertices, GameBoard* Board);
void BuildFlagVertices(std::vector<GLfloat>& Vertices, GameBoard* Board);
void UpdateTileColour(GameBoard* Board, GLuint VertexBuffer, int gridX, int gridY, int hitMineX = -1, int hitMineY = -1);
std::vector<GLfloat> BuildGridVertices(int Width, int Height, float Gap);
void BuildThreeDigitDisplay(std::vector<GLfloat>& Vertices, int value, float startX, float y, float digitSpacing);