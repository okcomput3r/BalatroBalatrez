#pragma once

#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

struct ImageData {
    GLuint shaderProgram = 0;
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    GLuint textureID = 0;
    int width = 0;
    int height = 0;
    float posX = 0.0f;
    float posY = 0.0f ;
    float targetX = 0.0f;
    float targetY = 0.0f;
};


bool InitImage(ImageData& img, const std::string& vertexPath, const std::string& fragmentPath, const std::string& texturePath);

// Dibuja la imagen en pantalla
void DrawImage(const ImageData& img, const glm::mat4& transform, const glm::vec2& uv_offset = glm::vec2(0.0f), const glm::vec2& uv_scale = glm::vec2(1.0f));

void DibujarImagen(ImageData img, glm::mat4 projection, glm::mat4 model);

void DestroyImage(ImageData& img);

void LoadImageBuffers(ImageData& img);
bool LoadImageShaders(ImageData& img, const std::string& vertexPath, const std::string& fragmentPath);
bool LoadImageTextures(ImageData& img, const std::string& texturePath);

