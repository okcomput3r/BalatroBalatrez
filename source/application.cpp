#include <application.h>
#include <Utils/logs.h>

// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <queue>

#include <dirent.h>

// Include the main libnx system header, for Switch development
#include <switch.h>

// GRAPHICS AND IMPUTS INCLUDES
#include <SDL.h>
#include <SDL_video.h>
#include <SDL_opengles2.h>
#include <GLES3/gl3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Custom headers

#include <Graphics/shaders.h>
#include <Graphics/textures.h>
#include <Graphics/cards.h>
#include <Utils/logs.h>
#include <Utils/math.h>


// some switch buttons
#define JOY_A     0
#define JOY_B     1
#define JOY_X     2
#define JOY_Y     3
#define JOY_PLUS  10
#define JOY_MINUS 11
#define JOY_LEFT  12
#define JOY_UP    13
#define JOY_RIGHT 14
#define JOY_DOWN  15


//SDL/GLES Global Contex

SDL_Event event;
SDL_Window *window;
SDL_GLContext GLcontext;  // GLES (OpenGL for Embeded System) context for shaders and whatever  


// GLOBAL 
std::vector<GLuint> globalDeck;
std::vector<GLuint> globalHand;

int APP::ConfigureApplication()
{
    TRACE("APP INITIALIZATION");


    // Innit romfs and logs
    romfsInit();

    if (innitLogs() == -1) { return -1; }

    if(SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
        TRACE("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    // Configs context

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES); // Set the profile of OpenGl to Embeded Systems
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1"); // Forces ES drivers

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    

    window = SDL_CreateWindow("Prueba de cartas", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN);
    if (!window) {
        TRACE("SDL_CreateWindow: %s\n", SDL_GetError());
        return -1;
    }

    GLcontext = SDL_GL_CreateContext(window);
    if(!GLcontext)
    {
        TRACE("SDL_GL_CreateContext: %s\n", SDL_GetError());
        return -1;
    }


    // Carga todas las funciones de OpenGl
    int version = gladLoadGLLoader(SDL_GL_GetProcAddress);
    if(version == 0) {
        TRACE("Unable To load GLAD: %s\n", SDL_GetError());
        return -1;
    }


    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    TRACE("APP INICIALIZATED");
  return 0;
}
GLuint cardHandle;

int APP::SetupScene()
{
    TRACE("START SETUP OF SCENE");

    // Cards.h
    LoadBaseCardBuffers();
    LoadBaseCardShader();
    LoadBaseCardTextures();

    LoadDeck(globalDeck);

    AddCardToHand(1, globalHand, globalDeck);
    AddCardToHand(2, globalHand, globalDeck);
    AddCardToHand(3, globalHand, globalDeck);
    AddCardToHand(5, globalHand, globalDeck);

    TRACE("SCENE SETTED");

    return 0;
}

float finalPos[3] = {SCREEN_W/2,SCREEN_H/2, 0.0f};
float endAngle = 180.0f;

uint32_t tick = SDL_GetTicks();
uint32_t last_tick = SDL_GetTicks();;
float delta_time;
void APP::Update()
{
    
    tick = SDL_GetTicks();
    // Cuántos segundos han pasado desde el último frame (ej. 0.016s a 60fps)
    delta_time = (tick - last_tick) / 1000.0f; 
    last_tick = tick;
    
    UpdateHand(globalHand);

    updateLogs();
}

void APP::Render()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    RenderHand(globalHand);
    RenderDeck(globalDeck);

    SDL_GL_SwapWindow(window);
    TRACE("END OF RENDER at time %d", SDL_GetTicks());
    TRACE("ms: %f", delta_time);
}


void APP::CleanApplication()
{   
    //destroyShaderProgram(cardBaseShader);
    //texDestroy(textureFront);
    //texDestroy(textureBack);
    terminateLogs();
    SDL_Quit();
}