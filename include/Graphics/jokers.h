#pragma once

#include <GLES3/gl3.h>
#include <vector>
#include <string>



// Tipos de efectos posibles que puede hacer un Joker
enum JokerEffectType {
    // Efectos incondicionales
    EFFECT_ADD_MULT,
    EFFECT_MULT_MULT,
    EFFECT_ADD_CHIPS,

    // Efectos condicionales por PALOS
    EFFECT_ADD_MULT_OROS,
    EFFECT_ADD_MULT_COPAS,
    EFFECT_ADD_MULT_ESPADAS,
    EFFECT_ADD_MULT_BASTOS,

    // Efectos condicionales por MANO (Añaden Multiplicador)
    EFFECT_ADD_MULT_PAREJA,
    EFFECT_ADD_MULT_TRIO,
    EFFECT_ADD_MULT_POKER,
    EFFECT_ADD_MULT_ESCALERA,
    EFFECT_ADD_MULT_COLOR,

    // Efectos condicionales por MANO (Añaden Fichas)
    EFFECT_ADD_CHIPS_PAREJA,
    EFFECT_ADD_CHIPS_TRIO,
    EFFECT_ADD_CHIPS_POKER,
    EFFECT_ADD_CHIPS_ESCALERA,
    EFFECT_ADD_CHIPS_COLOR,

    // Efectos especiales
    EFFECT_ADD_MULT_HALF_DECK, // +Mult si quedan menos de 3 cartas
    EFFECT_ECONOMY_DEBT,       // Tarjeta de crédito (-20 límite deuda)
    EFFECT_CHIPS_PER_DISCARD,  // Banner (+Fichas por descarte)
    EFFECT_RANDOM_MULT,        // Misprint (+Mult aleatorio)
    EFFECT_CHIPS_FACE_CARDS,   // Scary face (+Fichas por figura)
    EFFECT_MULT_PER_JOKER,     // Abstract Joker (+Mult por cada Joker)
    EFFECT_ECONOMY_END_ROUND   // Huevo (+Monedas al final de ronda)
};

typedef struct joker 
{
    int id;
    int atlasIndex; // Índice para saber qué sprite del atlas usar
    std::string name;
    
    JokerEffectType effectType; // Tipo de efecto que tiene el Joker
    float effectValue; 
    
} Joker;

extern std::vector<Joker> ownedJokers;
extern std::vector<Joker> jokerDatabase; // Base de datos de jokers disponibles en el juego, con sus efectos y características

