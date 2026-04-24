#pragma once

#include <Graphics/image.h>
#include <switch.h>
#include <vector>
#include <Utils/math.h>

struct JokerCard {
    int spriteID;
    float baseX; // La posición X original de reposo
    float posX, posY;
    float targetX, targetY;
};

struct StoreMenuState {
    ImageData uiAtlas;
    ImageData background;
    ImageData difuminado;
    bool arraySeleccionados[3] = {false, false, false};
    std::vector<JokerCard> jokers;
    int selectedIndex = 0;

    float menuY = 1500.0f;       
    float targetMenuY = 1500.0f;
};

// Funciones principales
void InitStoreMenu(StoreMenuState& storeState);
void UpdateStoreMenu(StoreMenuState& storeState, u64 botonesPulsados, float delta_time, bool& pausa);
void RenderStoreMenu(const StoreMenuState& storeState, const glm::mat4& projection, glm::mat4 model, ImageData img);

void RenderStoreMenuDescriptions(const StoreMenuState& storeState, const glm::mat4& projection, glm::mat4 model, bool Seleccionado[], int size, bool dibujar);