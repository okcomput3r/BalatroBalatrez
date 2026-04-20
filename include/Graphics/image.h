#pragma once

#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <unordered_map>


struct SpriteDef {
    glm::vec2 uv_offset;
    glm::vec2 uv_scale;
    float pixelWidth;
    float pixelHeight;
};

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

    std::unordered_map<int, SpriteDef> sprites;
};


bool InitImage(ImageData& img, const std::string& vertexPath, const std::string& fragmentPath, const std::string& texturePath);

// Dibuja la imagen en pantalla
void DrawImage(const ImageData& img, const glm::mat4& transform, const glm::vec2& uv_offset = glm::vec2(0.0f), const glm::vec2& uv_scale = glm::vec2(1.0f));

void DibujarImagen(ImageData img, glm::mat4 projection, glm::mat4 model);

void DestroyImage(ImageData& img);

void LoadImageBuffers(ImageData& img);
bool LoadImageShaders(ImageData& img, const std::string& vertexPath, const std::string& fragmentPath);
bool LoadImageTextures(ImageData& img, const std::string& texturePath);


void DefineAtlasSprite(ImageData& img, int spriteID, float pixelX, float pixelY, float width, float height);

void DrawAtlasSprite(const ImageData& img, int spriteID, const glm::mat4& projection, float posX, float posY, float scale = 1.0f);

void InitializeAtlas(ImageData& img, int col, int row, float sizeX, float sizeY);

void DrawText(const ImageData& fontAtlas, const std::string& text, const glm::mat4& projection, float posX, float posY, float scale = 1.0f, float letterSpacing = 0.0f);