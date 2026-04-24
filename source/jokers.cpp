#include <Graphics/jokers.h>
#include <GLES3/gl3.h>
#include <vector>
#include <string>

std::vector<Joker> ownedJokers;
std::vector<Joker> jokerDatabase = {
    // FORMATO: {ID, AtlasIndex, "Nombre", ACCION, Valor, DISPARADOR, "CondicionString", CondicionInt}

    // --- Básico ---
    {0, 0, "Joker", ACTION_ADD_MULT, 4.0f, TRIGGER_ALWAYS, "", -1},

    // --- Jokers de Palos
    {1, 16, "Greedy Joker", ACTION_ADD_MULT, 3.0f, TRIGGER_ON_SUIT, "", 3},
    {2, 17, "Lusty Joker", ACTION_ADD_MULT, 3.0f, TRIGGER_ON_SUIT, "", 2},
    {3, 18, "Wrathful Joker", ACTION_ADD_MULT, 3.0f, TRIGGER_ON_SUIT, "", 1},
    {4, 19, "Gluttonous Joker", ACTION_ADD_MULT, 3.0f, TRIGGER_ON_SUIT, "", 0},

    // --- Jokers de Manos Específicas (+Mult) ---
    {5, 2, "Jolly Joker", ACTION_ADD_MULT, 8.0f, TRIGGER_ON_HAND, "Pareja", -1},
    {6, 3, "Zany Joker", ACTION_ADD_MULT, 12.0f, TRIGGER_ON_HAND, "Trio", -1},
    {7, 4, "Mad Joker", ACTION_ADD_MULT, 10.0f, TRIGGER_ON_HAND, "Doble Pareja", -1},
    {8, 5, "Crazy Joker", ACTION_ADD_MULT, 12.0f, TRIGGER_ON_HAND, "Escalera", -1},
    {9, 6, "Droll Joker", ACTION_ADD_MULT, 10.0f, TRIGGER_ON_HAND, "Color", -1},

    // --- Jokers de Manos Específicas (+Chips) ---
    {10, 140, "Sly Joker", ACTION_ADD_CHIPS, 50.0f, TRIGGER_ON_HAND, "Pareja", -1},
    {11, 141, "Wily Joker", ACTION_ADD_CHIPS, 100.0f, TRIGGER_ON_HAND, "Trio", -1},
    {12, 142, "Clever Joker", ACTION_ADD_CHIPS, 80.0f, TRIGGER_ON_HAND, "Doble Pareja", -1},
    {13, 143, "Devious Joker", ACTION_ADD_CHIPS, 100.0f, TRIGGER_ON_HAND, "Escalera", -1},
    {14, 144, "Crafty Joker", ACTION_ADD_CHIPS, 80.0f, TRIGGER_ON_HAND, "Color", -1},

    // --- Jokers de Condiciones Especiales ---
    // Half Joker: +20 Mult si juegas 3 cartas o menos. Usamos CondicionInt = 3.
    {15, 7, "Half Joker", ACTION_ADD_MULT, 20.0f, TRIGGER_MAX_CARDS, "", 3},
    
    // Credit Card: Permite tener hasta -$20. Esto afecta a la tienda, no a los puntos.
    {16, 15, "Credit Card", ACTION_ECONOMY, 15.0f, TRIGGER_ON_ACQUIRE, "Debt", -1},
    
    // Banner: +40 Chips por cada descarte restante
    {17, 21, "Banner", ACTION_ADD_CHIPS, 30.0f, TRIGGER_PER_DISCARD, "", -1},
    
    // Scary Face: +30 Chips por cada sota, caballo o rey
    {18, 32, "Scary Face", ACTION_ADD_CHIPS, 30.0f, TRIGGER_ON_FACE_CARD, "", -1},
    
    // Abstract Joker: +3 Mult por cada Joker que tengas
    {19, 33, "Abstract Joker", ACTION_ADD_MULT, 3.0f, TRIGGER_PER_JOKER, "", -1},
    
    // Huevo: Ganas dinero al final de la ronda (Ciega superada)
    {20, 100, "Huevo", ACTION_ECONOMY, 3.0f, TRIGGER_END_ROUND, "", -1}
};