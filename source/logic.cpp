#include <logic.h>
#include <Graphics/cards.h>
#include <Graphics/jokers.h> 
#include <Utils/logs.h>

EstadoPartida estadoPartida;

void IniciarNuevaPartida() {
    estadoPartida.faseActual = PHASE_BLIND_SELECT;
    estadoPartida.dinero = 4;
    estadoPartida.puntuacionGlobal = 0;
    estadoPartida.ronda = 1;
}

void ConfigurarCiega(float objetivo) {
    estadoPartida.ciegaObjetivo = objetivo;
    estadoPartida.puntuacionGlobal = 0;
    estadoPartida.manos = 4;      
    estadoPartida.descartes = 3;   
    estadoPartida.faseActual = PHASE_PLAYING;
}

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

void CalcularPuntuacionPrevia(std::string nombreMano, const std::vector<unsigned int>& cartasSeleccionadas) {
    if (cartasSeleccionadas.empty()) {
        estadoPartida.fichas = 0;
        estadoPartida.mult = 0;
        return;
    }

    float chips = 0;
    float mult = 0;

    ObtenerValoresBaseMano(nombreMano, chips, mult);

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

    // Aplicar Jokers (PASADA 1: Sumas de Chips y Mult)
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

    // Aplicar Jokers (PASADA 2: Multiplicaciones de Mult)
    // Se hace en un bucle separado porque XMult siempre se aplica después de sumar todo
    for (size_t i = 0; i < ownedJokers.size(); i++) {
        Joker& joker = ownedJokers[i];
        if (joker.effectType == EFFECT_MULT_MULT) {
            mult *= joker.effectValue;
        }
    }

    // Actualizar para que se muestre en la interfaz
    estadoPartida.fichas = chips;
    estadoPartida.mult = mult;
}

void JugarMano(std::string nombreMano, const std::vector<unsigned int>& cartasSeleccionadas) {
    if (estadoPartida.manos <= 0) return; // No te quedan manos
    
    estadoPartida.manos--;
    estadoPartida.faseActual = PHASE_CALCULATING; // Bloquea los controles mientras calcula

    CalcularPuntuacionPrevia(nombreMano, cartasSeleccionadas);
    
    // Multiplicamos Fichas x Mult y lo sumamos al total
    float puntuacionMano = estadoPartida.fichas * estadoPartida.mult;
    estadoPartida.puntuacionGlobal += puntuacionMano;

    TRACE("Mano jugada: %s | Puntos obtenidos: %f | Total: %f / %f", 
          nombreMano.c_str(), puntuacionMano, estadoPartida.puntuacionGlobal, estadoPartida.ciegaObjetivo);

    // Comprobar si hemos ganado o perdido
    if (estadoPartida.puntuacionGlobal >= estadoPartida.ciegaObjetivo) {
        TRACE("¡CIEGA SUPERADA!");
        estadoPartida.faseActual = PHASE_SHOP;
    } else if (estadoPartida.manos == 0) {
        TRACE("GAME OVER");
        estadoPartida.faseActual = PHASE_GAME_OVER;
    } else {
        estadoPartida.faseActual = PHASE_PLAYING;
    }
}

void DescartarCartas() {
    if (estadoPartida.descartes > 0) {
        estadoPartida.descartes--;
    }
}

void AvanzarSiguienteCiega() {
    estadoPartida.ronda++;
    float nuevoObjetivo = estadoPartida.ciegaObjetivo * 1.5f; 
    estadoPartida.dinero += 5; 
    ConfigurarCiega(nuevoObjetivo);
}