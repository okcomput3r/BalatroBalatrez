#include <Graphics/image.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <Utils/logs.h>


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

void InitializeImage(ImageData& img, float positionX, float positionY, const std::string& png){

    if (!InitImage(img, "romfs:/data/shaders/cardBase.vert", "romfs:/data/shaders/imageBase.frag", "romfs:/data/textures/" + png)) {
        TRACE("WARNING: No se pudo cargar la imagen.");
    }
    img.posX = positionX;
    img.posY = positionY;

    img.targetX = positionX;
    img.targetY = positionY;

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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

void DibujarImagen(ImageData img, glm::mat4 projection, glm::mat4 model, float positionX, float positionY) {
    float PosX = positionX;
    float PosY = positionY;
    model = glm::translate(model, glm::vec3(PosX, PosY, 0.0f));
    model = glm::scale(model, glm::vec3((float)img.width, (float)img.height, 1.0f));

    glm::mat4 finalTransform = projection * model;
    DrawImage(img, finalTransform);
}


void DefineAtlasSprite(ImageData& img, int spriteID, float pixelX, float pixelY, float width, float height) {
    SpriteDef sprite;
    sprite.pixelWidth = width;
    sprite.pixelHeight = height;

    // Qué porcentaje del total del atlas ocupa este sprite (de 0.0 a 1.0)
    sprite.uv_scale.x = width / (float)img.width;
    sprite.uv_scale.y = height / (float)img.height;

    // En qué porcentaje empieza el recorte
    sprite.uv_offset.x = pixelX / (float)img.width;
    
    // Invertimos la Y para que coincida con la lectura de OpenGL
    sprite.uv_offset.y = 1.0f - ((pixelY + height) / (float)img.height);

    // Lo guardamos en el diccionario con su ID
    img.sprites[spriteID] = sprite;
}

void DrawAtlasSprite(const ImageData& img, int spriteID, const glm::mat4& projection, float posX, float posY, float scale) {
    // Comprobamos que el ID exista para no crashear
    if (img.sprites.find(spriteID) == img.sprites.end()) {
        std::cerr << "Error: Sprite ID " << spriteID << " no encontrado en el atlas." << std::endl;
        return;
    }

    // Recuperamos los datos del recorte
    SpriteDef sprite = img.sprites.at(spriteID);

    float finalWidth = sprite.pixelWidth * scale;
    float finalHeight = sprite.pixelHeight * scale;

    glm::mat4 model = glm::mat4(1.0f);
    
    // Usamos el tamaño final para calcular el centro exacto
    model = glm::translate(model, glm::vec3(posX + (finalWidth / 2.0f), posY + (finalHeight / 2.0f), 0.0f));
    
    // Aplicamos el tamaño final a la matriz de escala
    model = glm::scale(model, glm::vec3(finalWidth, finalHeight, 1.0f));

    glm::mat4 finalTransform = projection * model;

    // Llamamos al DrawImage pasándole los UVs calculados
    DrawImage(img, finalTransform, sprite.uv_offset, sprite.uv_scale);
}

void InitializeAtlas(ImageData& img, int col, int row, float sizeX, float sizeY){
    int columnas = col; // 15
    int filas = row;    // 8
    int id_actual = 0;

    for (int y = filas - 1; y >= 0; y--) {
        for (int x = 0; x < columnas; x++) {
            // Calculamos en qué píxel cae esta columna y fila
            float pixelX = x * sizeX; //20.0f;
            float pixelY = y * sizeY; //20.0f;

            // Registramos el sprite
            DefineAtlasSprite(img, id_actual, pixelX, pixelY, sizeX, sizeY);
            
            id_actual++;
        }
    }
}

void DrawText(const ImageData& fontAtlas, const std::string& text, const glm::mat4& projection, float posX, float posY, float scale, float letterSpacing) {

    float sizee = text.size();
    float currentX = posX - (sizee/2) * ((20.0f * scale) + letterSpacing);
    
    float currentY = posY;

    for (char c : text) {

        // Convertir el carácter ASCII a nuestro ID del atlas (ASCII 65 'A' - 32 = ID 33)
        int spriteID = (int)c - 32;

        // Comprobamos si el sprite existe en el diccionario
        if (fontAtlas.sprites.find(spriteID) != fontAtlas.sprites.end()) {
            SpriteDef sprite = fontAtlas.sprites.at(spriteID);
            
            // Si no es un espacio en blanco
            if (spriteID != 0) {
                DrawAtlasSprite(fontAtlas, spriteID, projection, currentX, currentY, scale);
            }
            
            // Avanzamos donde escribimos hacia la derecha para la siguiente letra
            currentX += (sprite.pixelWidth * scale) + letterSpacing;
        } else {
            // Si el carácter no existe en el atlas,
            // dejamos un espacio del ancho de la Apara no descolocar el texto.
            if (fontAtlas.sprites.find(33) != fontAtlas.sprites.end()) {
                currentX += (fontAtlas.sprites.at(33).pixelWidth * scale) + letterSpacing;
            }
        }
    }
}

