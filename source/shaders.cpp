// Shader.cpp

#include <Graphics/shaders.h>
#include <Utils/logs.h>

#include <cstdio>
#include <cstdlib>
#include <SDL.h>
#include <SDL_opengles.h>
#include <GLES3/gl3.h>
#include <glad/glad.h>

#if _MSC_VER
#pragma warning(disable:4996)
#endif

static size_t fileGetLenght(FILE *file)
{
    size_t lenght;
    size_t currentPos = ftell(file);

    fseek(file, 0, SEEK_END); // mueve el puntero del descriptor al final del archivo

    lenght = ftell(file);

    fseek(file, currentPos, SEEK_SET); // devuelve el puntero a su posición original

    return lenght;
} 

static GLuint shaderLoad (const char *shaderPath, const GLuint shaderType)
{
    TRACE("Loading shader %s", shaderPath);

    FILE *file = fopen(shaderPath, "r");
    if(!file)
    {   
        TRACE("Couldn't open file");
        return 0;}

    size_t fileLenght = fileGetLenght(file);

    GLchar *shaderSrc = (GLchar*) calloc (fileLenght + 1, 1); 

    if (!shaderSrc)
    {
        TRACE("Out of memory, couldn't alloc shader");
        fclose(file);
        return 0;
    }

    // fread() lee de la dirección de memoria "file" 
    // un numero lenght de datos, de 1 byte de tamaño,
    // y lo almacena en el vector "shaderSrc"

    fread(shaderSrc, 1, fileLenght, file);
    fclose(file);

    GLuint shaderHandle = glCreateShader(shaderType);
    glShaderSource(shaderHandle, 1, &shaderSrc, nullptr); // asocia el source al shader

    // Compilar el shader
    glCompileShader(shaderHandle);
    GLint compileSuccess = GL_FALSE;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);

    if(!compileSuccess)
    {
        TRACE("Compilation of shader %s failed \nsource: \n\n%s\n", shaderPath, shaderSrc);
        GLint logLength = 0;

        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLength);
        GLchar *errlog = (GLchar*) malloc(logLength);

        if(errlog)
        {
            glGetShaderInfoLog(shaderHandle, logLength, &logLength, errlog);
            TRACE("%s\n", errlog);
            free(errlog);
        }
        else 
        {
            TRACE("Couldn't open log.");
        }

        glDeleteShader(shaderHandle);
        shaderHandle = 0;

        
    }

    free(shaderSrc);
    shaderSrc = NULL;

    TRACE("Shader %s Loaded with ID: %d", shaderPath, shaderHandle);

    return shaderHandle;
}

void shaderDestroy(GLuint shaderId)
{
    TRACE("Destroying Shader with ID: %d", shaderId);
    glDeleteShader(shaderId);
}

GLuint compileShaderProgram(const char *vertexPath, const char *fragmentPath)
{
    TRACE("Starting Compilation of Shader program: %s \t %s", vertexPath, fragmentPath);

    GLuint vertexShader = shaderLoad(vertexPath, GL_VERTEX_SHADER);
    if(!vertexShader)
    {
        TRACE("Couldn't load vertexShader");
        return 0;
    }

    GLuint fragmentShader = shaderLoad(fragmentPath, GL_FRAGMENT_SHADER);
    if(!fragmentShader)
    {
        TRACE("Couldn't load fragmentShader");
        return 0;
    }

    // Creación del programa shader
    GLuint shaderProgram = glCreateProgram();
    if(shaderProgram)
    {
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);

        glLinkProgram(shaderProgram);

        GLint linkingSucceeded = GL_FALSE;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkingSucceeded);
        
        if (!linkingSucceeded) {
            
            TRACE("Linking shader failed (vert. shader: %s, frag. shader: %s\n", vertexPath, fragmentPath);
            
            GLint logLength = 0;
            
            glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
            GLchar *errLog = (GLchar*)malloc(logLength);
            if (errLog) {
                glGetProgramInfoLog(shaderProgram, logLength, &logLength, errLog);
                
                TRACE("%s\n", errLog);
                free(errLog);
            }
            else {
                TRACE("Couldn't get shader link log; out of memory\n");
            }
            
            glDeleteProgram(shaderProgram);
            
            shaderProgram = 0;
            
        }
    }
    else 
    {
        TRACE("Couldn't create shader program");
    }

    shaderDestroy(vertexShader);
    shaderDestroy(fragmentShader);
    
    TRACE("Compilation of Shader program %s \t %s successfull. ID: %d", vertexPath, fragmentPath, shaderProgram);

    return shaderProgram;
}

void destroyShaderProgram(GLuint shaderID)
{
    TRACE("Destroying Shader Program with ID: %d", shaderID);
    glDeleteProgram(shaderID);
}