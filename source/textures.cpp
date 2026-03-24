#include <Graphics/textures.h>
#include <Utils/logs.h>

#include <GLES3/gl3.h>
#include <SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL_opengles.h>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


// función cortesía del curso online que estoy leyendo
/** Sets the swizzling for a texture colour channel from an SDL colour mask.
*
* @param channel the texture channel to set (e.g., GL_TEXTURE_SWIZZLE_R)
* @param mask the SDL colour channel mask (e.g., texSurf->format->Rmask)
*/

/*
bool sdlToGLSwizzle(GLenum channel, Uint32 mask) {
    GLint swizzle;
    
    switch (mask) {
        case 0x000000FF:
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    
    swizzle = GL_ALPHA;
    
    #else
    
    swizzle = GL_RED;
    
    #endif
    break;
        case 0x0000FF00:
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    
    swizzle = GL_BLUE;
    
    #else
    
    swizzle = GL_GREEN;
    
    #endif
    break;
        case 0x00FF0000:
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    
    swizzle = GL_GREEN;
    
    #else
    
    swizzle = GL_BLUE;

    #endif
    break;
    
        case 0xFF000000:
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    
    swizzle = GL_ALPHA;
    
    #else
    
    swizzle = GL_RED;
    
    #endif
    break;
        default:
    SDL_Log("Unrecognized colour channel mask 0x%08X", mask);
    return false;
    }
    glTexParameteri(GL_TEXTURE_2D, channel, swizzle);
    return true;
}
*/

GLuint loadTexture(const char *filename)
{
    GLuint handle;

    /*
    int flags = IMG_INIT_JPG | IMG_INIT_PNG;

    if((IMG_Init(flags) & flags) == 0)
    {
        TRACE( "ERROR: Texture loading failed. Couldn't get JPEG and PNG loaders.\n");
        return 0;
    }
    */

    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0); 
    if(!data)
    {
        TRACE("Loading image %s failed", filename);
        stbi_image_free(data);
        return 0;
    }

    /*
    SDL_Surface *texSurf = IMG_Load(filename);
    if(!texSurf)
    {
        TRACE("Loading image %s failed with error: %s", filename, IMG_GetError());
        return 0;
    }

    
    // Determine the format
    // NOTE: Only supporting 24 and 32-bit images
    GLenum format;
    GLenum type = GL_UNSIGNED_BYTE;
    switch (texSurf->format->BytesPerPixel) {
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            TRACE("Can't load image %s; it isn't a 24/32-bit image\n", filename);
            SDL_FreeSurface(texSurf);
            texSurf = NULL;
            return 0;
    }
    */
    // Generate the texture

    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);
    //glTexImage2D(GL_TEXTURE_2D, 0, format, texSurf->w, texSurf->h, 0, format, type, texSurf->pixels);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    GLenum err = glGetError();
    if(err != GL_NO_ERROR)
    {
        glDeleteBuffers(1, &handle);
        stbi_image_free(data);

        //SDL_FreeSurface(texSurf);
        TRACE("Creating texture %s failed, code %u\n", filename, err);
        return 0;
    }


    // Set up texture swizzling to match the image's channel order
    /*
    bool success = sdlToGLSwizzle(GL_TEXTURE_SWIZZLE_R, texSurf->format->Rmask);
    success &= sdlToGLSwizzle(GL_TEXTURE_SWIZZLE_G, texSurf->format->Gmask);
    success &= sdlToGLSwizzle(GL_TEXTURE_SWIZZLE_B, texSurf->format->Bmask);
    

    if (format == GL_RGBA) {
        success &= sdlToGLSwizzle(GL_TEXTURE_SWIZZLE_A, texSurf->format->Amask);
    }
    
    if (!success) {
        TRACE("Couldn't set up swizzling for texture %s\n", filename);
        glDeleteBuffers(1, &handle);
        handle = 0;
        //SDL_FreeSurface(texSurf);
        //texSurf = NULL;
        return 0;
    }
    */
    // Set up the filtering
    // NOTE: Failure to do this may result in no texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    stbi_image_free(data);

    //SDL_FreeSurface(texSurf);

    TRACE("Generation of texture %s with ID: %d", filename, handle);

    return handle;
}

void texDestroy(GLuint textureID)
{
    TRACE("Destroying texture with ID: %d", textureID);

    glDeleteBuffers(1, &textureID);
}