#pragma once

#include <Graphics/image.h>
#include <switch.h>
#include <vector>

// Estructura ligera solo para las matemáticas del botón
struct PauseButton {
    int spriteID;
    float baseX; // La posición X original de reposo
    float posX, posY;
    float targetX, targetY;
};

// El estado general de tu menú de pausa
struct PauseMenuState {
    ImageData uiAtlas; // Aquí cargamos el fondo y los botones
    ImageData background;
    ImageData difuminado;
    std::vector<PauseButton> buttons;
    int selectedIndex = 0;

    float menuY = 1500.0f;       
    float targetMenuY = 1500.0f;
};

// Funciones principales
void InitPauseMenu(PauseMenuState& pauseState);
void UpdatePauseMenu(PauseMenuState& pauseState, u64 botonesPulsados, float delta_time);
void RenderPauseMenu(const PauseMenuState& pauseState, const glm::mat4& projection, glm::mat4 model, ImageData img);
//void RenderPauseMenuDescriptions(const PauseMenuState& pauseState, const glm::mat4& projection, glm::mat4 model, bool Seleccionado);