#pragma once

#include <GLES3/gl3.h>

GLuint loadTexture(const char *filename);

void texDestroy(GLuint textureID);