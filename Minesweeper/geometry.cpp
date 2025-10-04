#include "geometry.h"


void CreateNumberQuad(std::vector<GLfloat>& vertices, int gridX, int gridY, int number) //AI
{
    // Position the number in the center of the tile
    float xPos = gridX + 0.1f;
    float yPos = gridY + 0.1f;
    float size = 0.8f;  // Number takes up 40% of tile

    // Calculate UV coordinates for this digit (0-7 for digits 1-8)
    float uLeft = (number - 1) / 8.0f;
    float uRight = number / 8.0f;

    // Two triangles forming a quad
    // Vertex format: x, y, z, u, v

    // Triangle 1
    vertices.push_back(xPos);           vertices.push_back(yPos);           vertices.push_back(0.0f);
    vertices.push_back(uLeft);          vertices.push_back(1.0f);  // Bottom-left

    vertices.push_back(xPos + size);    vertices.push_back(yPos);           vertices.push_back(0.0f);
    vertices.push_back(uRight);         vertices.push_back(1.0f);  // Bottom-right

    vertices.push_back(xPos + size);    vertices.push_back(yPos + size);    vertices.push_back(0.0f);
    vertices.push_back(uRight);         vertices.push_back(0.0f);  // Top-right

    // Triangle 2
    vertices.push_back(xPos);           vertices.push_back(yPos);           vertices.push_back(0.0f);
    vertices.push_back(uLeft);          vertices.push_back(1.0f);  // Bottom-left

    vertices.push_back(xPos);           vertices.push_back(yPos + size);    vertices.push_back(0.0f);
    vertices.push_back(uLeft);          vertices.push_back(0.0f);  // Top-left

    vertices.push_back(xPos + size);    vertices.push_back(yPos + size);    vertices.push_back(0.0f);
    vertices.push_back(uRight);         vertices.push_back(0.0f);  // Top-right
}

void CreateFlagQuad(float gridX, float gridY, std::vector<GLfloat>& vertices) {
    float size = 0.8f;
    float offsetX = 0.1f;
    float offsetY = 0.1f;

    float x1 = gridX + offsetX;
    float y1 = gridY + offsetY;
    float x2 = x1 + size;
    float y2 = y1 + size;

    float uLeft = 0.0f;
    float uRight = 1.0f;
    float vBottom = 1.0f;
    float vTop = 0.0f;

    // Triangle 1 - Vertex 1
    vertices.push_back(x1);      vertices.push_back(y1);      vertices.push_back(0.0f);
    vertices.push_back(uLeft);   vertices.push_back(vBottom);

    // Triangle 1 - Vertex 2
    vertices.push_back(x2);      vertices.push_back(y1);      vertices.push_back(0.0f);
    vertices.push_back(uRight);  vertices.push_back(vBottom);

    // Triangle 1 - Vertex 3
    vertices.push_back(x2);      vertices.push_back(y2);      vertices.push_back(0.0f);
    vertices.push_back(uRight);  vertices.push_back(vTop);

    // Triangle 2 - Vertex 1
    vertices.push_back(x1);      vertices.push_back(y1);      vertices.push_back(0.0f);
    vertices.push_back(uLeft);   vertices.push_back(vBottom);

    // Triangle 2 - Vertex 2
    vertices.push_back(x2);      vertices.push_back(y2);      vertices.push_back(0.0f);
    vertices.push_back(uRight);  vertices.push_back(vTop);

    // Triangle 2 - Vertex 3
    vertices.push_back(x1);      vertices.push_back(y2);      vertices.push_back(0.0f);
    vertices.push_back(uLeft);   vertices.push_back(vTop);
}

void CreateDigitQuad(std::vector<GLfloat>& vertices, float x, float y, int digit, float size)
{
    // UV coordinates for this digit (0-9)
    float uLeft = digit / 10.0f;
    float uRight = (digit + 1) / 10.0f;

    float x1 = x;
    float y1 = y;
    float x2 = x + size;
    float y2 = y + size;

    // Triangle 1
    vertices.push_back(x1);     vertices.push_back(y1);     vertices.push_back(0.0f);
    vertices.push_back(uLeft);  vertices.push_back(1.0f);

    vertices.push_back(x2);     vertices.push_back(y1);     vertices.push_back(0.0f);
    vertices.push_back(uRight); vertices.push_back(1.0f);

    vertices.push_back(x2);     vertices.push_back(y2);     vertices.push_back(0.0f);
    vertices.push_back(uRight); vertices.push_back(0.0f);

    // Triangle 2
    vertices.push_back(x1);     vertices.push_back(y1);     vertices.push_back(0.0f);
    vertices.push_back(uLeft);  vertices.push_back(1.0f);

    vertices.push_back(x2);     vertices.push_back(y2);     vertices.push_back(0.0f);
    vertices.push_back(uRight); vertices.push_back(0.0f);

    vertices.push_back(x1);     vertices.push_back(y2);     vertices.push_back(0.0f);
    vertices.push_back(uLeft);  vertices.push_back(0.0f);
}