#pragma once

#include <GLES3/gl3.h>
#include <glad/glad.h>
#include <Graphics/image.h>
#include <switch.h>
#include <vector>
#include <Utils/math.h>
#include <Graphics/jokers.h>


#include <string>


struct JokerCard {
    int spriteID;
    float baseY; // La posición X original de reposo
    float posX, posY;
    float targetX, targetY;
};

struct StoreMenuState {
    ImageData atlasJokers;
    ImageData descripcionJokers;
    ImageData background;
    ImageData comprar;
    bool arraySeleccionados[3] = {false, false, false};
    std::vector<JokerCard> jokers;
    int selectedIndex = 0;

    float menuY = 1500.0f;       
    float targetMenuY = 1500.0f;
};

// Funciones principales
void InitJokersStore(StoreMenuState& storeState, int random1, int random2, int random3);
void InitStoreMenu(StoreMenuState& storeState, int random1, int random2, int random3);

void UpdateStoreMenu(StoreMenuState& storeState, u64 botonesPulsados, float delta_time, bool& pausa, int random1, int random2, int random3);

void RenderStoreMenu(const StoreMenuState& storeState, const glm::mat4& projection, glm::mat4 model, ImageData img);

void RenderStoreMenuDescriptions(const StoreMenuState& storeState, const glm::mat4& projection, glm::mat4 model, bool Seleccionado[], int size, bool dibujar);
void GenerarNuevosJokersTienda(int& random1, int& random2, int& random3);