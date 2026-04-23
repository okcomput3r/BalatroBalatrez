#include "logic.h"
#include <Graphics/cards.h>
#include <Graphics/jokers.h> 
#include <Utils/logs.h>


// Definimos la variable global
EstadoPartida estadoPartida;

void IniciarNuevaPartida() {
    estadoPartida.faseActual = PHASE_BLIND_SELECT;
    estadoPartida.dinero = 0;
    estadoPartida.puntuacionGlobal = 0;
    estadoPartida.ronda = 0;
    // La configuración de manos y descartes base se hace al elegir la ciega
}

void ConfigurarCiega(float objetivo) {
    estadoPartida.ciegaObjetivo = objetivo;
    estadoPartida.puntuacionGlobal = 0;
    estadoPartida.manos = 4;      
    estadoPartida.descartes = 4;   
    estadoPartida.faseActual = PHASE_PLAYING;
}

// Lógica simulada de valores 
void ObtenerValoresBaseMano(std::string nombreMano, float &baseChips, float &baseMult) {
    if (nombreMano == "Escalera de Color") { baseChips = 100; baseMult = 8; }
    else if (nombreMano == "Poker") { baseChips = 60; baseMult = 7; }
    else if (nombreMano == "Full House") { baseChips = 40; baseMult = 4; }
    else if (nombreMano == "Color") { baseChips = 35; baseMult = 4; }
    else if (nombreMano == "Escalera") { baseChips = 30; baseMult = 4; }
    else if (nombreMano == "Trio") { baseChips = 30; baseMult = 3; }
    else if (nombreMano == "Doble Pareja") { baseChips = 20; baseMult = 2; }
    else if (nombreMano == "Pareja") { baseChips = 10; baseMult = 2; }
    else { baseChips = 5; baseMult = 1; } // Carta Alta
}

void JugarMano(std::string nombreMano, const std::vector<unsigned int>& cartasSeleccionadas) {
    if (estadoPartida.manos <= 0) return; // No te quedan manos
    
    estadoPartida.manos--;
    estadoPartida.faseActual = PHASE_CALCULATING; // Bloquea los controles mientras calcula

    float chips = 0;
    float mult = 0;

    // 1. Valores base de la mano jugada
    ObtenerValoresBaseMano(nombreMano, chips, mult);

    // 2. Sumar el valor de cada carta jugada a las Fichas
    for (unsigned int cardId : cartasSeleccionadas) {
        uint8_t result;
        Card &c = RetrieveCardReference(cardId, result);
        
        if (result) {
            int valorCarta = 0;
            
            if (c.level >= 2 && c.level <= 9) {
                valorCarta = c.level;
            } else if (c.level >= 10 && c.level <= 12) {
                valorCarta = 10; // Sota, Caballo y Rey valen 10
            } else if (c.level == 1) { // Depende de cómo definas el As
                valorCarta = 11; // El As vale 11
            }

            chips += valorCarta;
        }
    }

    // 3. Aplicar Jokers (PASADA 1: Sumas de Chips y Mult)
    for (size_t i = 0; i < ownedJokers.size(); i++) {
        Joker& joker = ownedJokers[i];

        switch (joker.effectType) {
            case EFFECT_ADD_MULT:
                mult += joker.effectValue;
                break;
                
            case EFFECT_ADD_CHIPS:
                chips += joker.effectValue;
                break;

            case EFFECT_ADD_MULT_PAREJA:
                if (nombreMano.find("Pareja") != std::string::npos || nombreMano == "Full House") {
                    mult += joker.effectValue;
                }
                break;

            case EFFECT_ADD_MULT_OROS:
                for (unsigned int cardId : cartasSeleccionadas) {
                    uint8_t result;
                    Card &c = RetrieveCardReference(cardId, result);
                    if (result && c.house == OROS) { 
                        mult += joker.effectValue;
                    }
                }
                break;

            // (Aquí puedes ir añadiendo los demás casos: EFFECT_ADD_CHIPS_PAREJA, EFFECT_ADD_MULT_ESPADAS, etc.)
        }
    }

    // 3.5 Aplicar Jokers (PASADA 2: Multiplicaciones de Mult)
    // Se hace en un bucle separado porque XMult siempre se aplica después de sumar todo
    for (size_t i = 0; i < ownedJokers.size(); i++) {
        Joker& joker = ownedJokers[i];
        if (joker.effectType == EFFECT_MULT_MULT) {
            mult *= joker.effectValue;
        }
    }

    // 4. Actualizar estado y sumar a la puntuación total
    estadoPartida.fichas = chips;
    estadoPartida.mult = mult;
    
    float puntuacionMano = chips * mult;
    estadoPartida.puntuacionGlobal += puntuacionMano;

    TRACE("Mano jugada: %s | Puntos obtenidos: %f | Total: %f / %f", 
          nombreMano.c_str(), puntuacionMano, estadoPartida.puntuacionGlobal, estadoPartida.ciegaObjetivo);

    // Comprobar si hemos ganado o perdido
    if (estadoPartida.puntuacionGlobal >= estadoPartida.ciegaObjetivo) {
        TRACE("¡CIEGA SUPERADA!");
        // Aquí darías el dinero y pasarías a la tienda
        estadoPartida.faseActual = PHASE_SHOP;
    } else if (estadoPartida.manos == 0) {
        TRACE("GAME OVER");
        estadoPartida.faseActual = PHASE_GAME_OVER;
    } else {
        // Volver a dar el control al jugador
        estadoPartida.faseActual = PHASE_PLAYING;
    }
}

void DescartarCartas() {
    if (estadoPartida.descartes > 0) {
        estadoPartida.descartes--;
        // La lógica de quitar las cartas de la mano real ocurrirá en application.cpp
    }
}