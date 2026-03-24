// Shader.h

#pragma once

#include <GLES3/gl3.h>


/**
 *  
 *
 *
 */
GLuint compileShaderProgram(const char *vertexPath, const char *fragmentPath);

void destroyShaderProgram(GLuint shaderID);