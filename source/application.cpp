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
#include <Utils/audio.h>

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


// switch pad
PadState pad;

//SDL/GLES Global Contex

SDL_Event event;
SDL_Window *window;
SDL_GLContext GLcontext;  // GLES (OpenGL for Embeded System) context for shaders and whatever  

// GLOBAL 
std::vector<GLuint> globalDeck;
std::vector<GLuint> globalHand;
int cursor;

Mix_Chunk* s_sfxCardSelect = nullptr;
Mix_Chunk* s_sfxCardUnselect = nullptr;

uint8_t maxSelectedCards = 5;
uint8_t selectedCardsCount = 0;

uint8_t handSize = 8;

int APP::ConfigureApplication()
{
    TRACE("APP INITIALIZATION");


    // Innit romfs and logs
    romfsInit();

    if (innitLogs() == -1) { return -1; }

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0 )
    {
        TRACE("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    // Initialize audio module
    Audio::Init();

    // Configs context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES); 
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1"); 

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

    // configuración del pad 1
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);


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

    

    for (int i = 1; i <= handSize; i++ )
    {
        //AddCardToHand(i, globalHand, globalDeck);
        DrawTopCard(globalHand, globalDeck);
    }

    SortHand(globalHand);

    // Posiciona el cursor al principio de la baraja
    cursor = 0;

    // Load SFX into memory
    s_sfxCardSelect = Audio::LoadSFX("romfs:/data/audio/card_select.wav");
    s_sfxCardUnselect = Audio::LoadSFX("romfs:/data/audio/card_unselect.wav");

    // Load and play background music only when the scene is setted up correctly
    Audio::PlayBGM("romfs:/data/audio/Balatro_Theme.mp3", 60);
    Audio::SetVolume(60); // Volume already set in PlayBGM, but can be adjusted later with this function

    TRACE("SCENE SETTED AND MUSIC STARTED");

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
    

    padUpdate(&pad);
    u64 botonesPulsados = padGetButtonsDown(&pad);

    
    // Mover el cursor a la Izquierda
    if (botonesPulsados & HidNpadButton_Left) {
        if (cursor > 0) {
            cursor--; // Nos movemos una carta a la izquierda
        }
    }

    // Mover el cursor a la Derecha
    if (botonesPulsados & HidNpadButton_Right) {
        // Asegurarnos de no salirnos de la mano
        if ((size_t) cursor < globalHand.size() - 1) { 
            cursor++; 
        }
    }

    
    // Seleccionar/Deseleccionar con la 'A'
    if (botonesPulsados & HidNpadButton_A) {
        // añadimos o substraemos la carta seleccionada al contador dependiendo si es true o false el valor selected
        uint8_t result;
        Card &cardRef = RetrieveCardReference(globalHand[cursor], result);

        if( (selectedCardsCount >= 0 && cardRef.selected) || (selectedCardsCount < maxSelectedCards && !cardRef.selected) )
        {

            if(!cardRef.selected) 
            {
                TRACE("Selecting Card with ID %d", cardRef.ID);

                selectedCardsCount++;

                Audio::PlaySFX(s_sfxCardSelect, 60);
            }else{
                TRACE("Unselecting Card with ID %d", cardRef.ID);

                selectedCardsCount--;

                Audio::PlaySFX(s_sfxCardUnselect, 60);
            }
            cardRef.selected = !cardRef.selected;
        
        }
    }

    // borra las cartas seleccionadas
    if (botonesPulsados & HidNpadButton_X)
    {
        if(selectedCardsCount != 0)
        {
            RemoveCardsFromHand(globalHand);
            selectedCardsCount = 0;
        }
        
       // si el tamaño de la baraja es menor que el tamaño real de la baraja
        if(globalDeck.size() > 0 && globalHand.size() < handSize)
        {
            while(globalHand.size() < handSize )
            {
                TRACE("MOVING CARD FROM HAND TO DECK. DECK SIZE: %ld", globalDeck.size());
                if(globalDeck.size() == 0) {break;}
                //AddCardToHand(globalDeck[globalDeck.size()-1], globalHand, globalDeck);
                DrawTopCard(globalHand, globalDeck);
                SortHand(globalHand);
                Audio::PlaySFX(s_sfxCardSelect, 60);
            }
        }

        cursor = 0;

    }

    UpdateHand(globalHand, cursor);

    updateLogs();
}

void APP::Render()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    RenderHand(globalHand);
    RenderDeck(globalDeck);

    SDL_GL_SwapWindow(window);
    //TRACE("END OF RENDER at time %d", SDL_GetTicks());
    //TRACE("ms: %f", delta_time);
}


void APP::CleanApplication()
{   
    //destroyShaderProgram(cardBaseShader);
    //texDestroy(textureFront);
    //texDestroy(textureBack);
    Audio::FreeSFX(s_sfxCardSelect);
    Audio::FreeSFX(s_sfxCardUnselect);
    Audio::Clean(); 
    terminateLogs();
    SDL_Quit();
}