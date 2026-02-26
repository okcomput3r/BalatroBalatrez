// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Include the main libnx system header, for Switch development
#include <switch.h>

// UNTESTED INCLUDES
#include <SDL.h>
#include <SDL_video.h>
#include <SDL_opengles2.h>
#include <GLES3/gl3.h>
#include <glad/glad.h>

#define SCREEN_W 1280
#define SCREEN_H 720


// Main program entrypoint
int main(int argc, char *argv[]) {
 

  //SDL_VideoInfo* info = NULL;  
  SDL_Event event;
  SDL_Window *window;
  SDL_Renderer *renderer;

  // GLES (OpenGL for Embeded System) context for shaders and whatever  
  SDL_GLContext GLcontext;


  if(SDL_Init(SDL_INIT_VIDEO) < 0 )
  {
    SDL_Log("SDL_Init: %s\n", SDL_GetError());
    return -1;
  }

  /* CONTEXT CONFIG */

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES); // Set the profile of OpenGl to Embeded Systems
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

  // Fuerza el uso del backend de GLES
  SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1");

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  

  // Crea la ventana 
  window = SDL_CreateWindow("Prueba de cartas", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN);
  if (!window) {
    SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
    return -1;
  }

  GLcontext = SDL_GL_CreateContext(window);
  if(!GLcontext)
  {
    SDL_Log("SDL_GL_CreateContext: %s\n", SDL_GetError());
    return -1;
  }


  // Carga todas las funciones de OpenGl
  int version = gladLoadGLLoader(SDL_GL_GetProcAddress);
  if(version == 0) {
    SDL_Log("Unable To load GLAD: %s\n", SDL_GetError());
    return -1;
  }


  while(true)
  {
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT);
  
    // Update the window
    SDL_GL_SwapWindow(window);
  }

  SDL_Quit();
  return 0;
}
