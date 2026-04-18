#include <Graphics/image.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "stb_image.h" 

static std::string ReadFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static GLuint CompileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}



bool InitImage(ImageData& img, const std::string& vertexPath, const std::string& fragmentPath, const std::string& texturePath) {
    if (!LoadImageShaders(img, vertexPath, fragmentPath)) return false;
    LoadImageBuffers(img);
    if (!LoadImageTextures(img, texturePath)) return false;
    return true;
}

void DestroyImage(ImageData& img) {
    glDeleteVertexArrays(1, &img.VAO);
    glDeleteBuffers(1, &img.VBO);
    glDeleteBuffers(1, &img.EBO);
    glDeleteProgram(img.shaderProgram);
    glDeleteTextures(1, &img.textureID);
}

void LoadImageBuffers(ImageData& img) {
    float vertices[] = {
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  
    };
    unsigned int indices[] = {  
        0, 1, 3, 
        1, 2, 3  
    };

    glGenVertexArrays(1, &img.VAO);
    glGenBuffers(1, &img.VBO);
    glGenBuffers(1, &img.EBO);

    glBindVertexArray(img.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, img.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, img.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); 
}

bool LoadImageShaders(ImageData& img, const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode = ReadFile(vertexPath);
    std::string fragmentCode = ReadFile(fragmentPath);

    if (vertexCode.empty() || fragmentCode.empty()) return false;

    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexCode);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentCode);

    img.shaderProgram = glCreateProgram();
    glAttachShader(img.shaderProgram, vertexShader);
    glAttachShader(img.shaderProgram, fragmentShader);
    glLinkProgram(img.shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(img.shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(img.shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

bool LoadImageTextures(ImageData& img, const std::string& texturePath) {
    glGenTextures(1, &img.textureID);
    glBindTexture(GL_TEXTURE_2D, img.textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //stbi_set_flip_vertically_on_load(false);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        img.width = width;
        img.height = height;
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Fallo al cargar la textura: " << texturePath << std::endl;
        stbi_image_free(data);
        //stbi_set_flip_vertically_on_load(false);
        return false;
    }
    stbi_image_free(data);
    //stbi_set_flip_vertically_on_load(false);
    return true;
}

void DrawImage(const ImageData& img, const glm::mat4& transform, const glm::vec2& uv_offset, const glm::vec2& uv_scale) {
    glUseProgram(img.shaderProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, img.textureID);
    glUniform1i(glGetUniformLocation(img.shaderProgram, "fontTexture"), 0);

    glUniformMatrix4fv(glGetUniformLocation(img.shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transform));
    glUniform2fv(glGetUniformLocation(img.shaderProgram, "uv_offset"), 1, glm::value_ptr(uv_offset));
    glUniform2fv(glGetUniformLocation(img.shaderProgram, "uv_scale"), 1, glm::value_ptr(uv_scale));

    glBindVertexArray(img.VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void DibujarImagen(ImageData img, glm::mat4 projection, glm::mat4 model) {
    float PosX = img.posX;
    float PosY = img.posY;
    model = glm::translate(model, glm::vec3(PosX, PosY, 0.0f));
    model = glm::scale(model, glm::vec3((float)img.width, (float)img.height, 1.0f));

    glm::mat4 finalTransform = projection * model;
    DrawImage(img, finalTransform);
}

