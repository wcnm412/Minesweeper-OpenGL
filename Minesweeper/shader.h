#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);