#pragma once

#include <vector>
#include <glad/glad.h>  // For GLfloat type

void CreateNumberQuad(std::vector<GLfloat>& vertices, int gridX, int gridY, int number);
void CreateFlagQuad(float gridX, float gridY, std::vector<GLfloat>& vertices);
void CreateDigitQuad(std::vector<GLfloat>& vertices, float x, float y, int digit, float size);