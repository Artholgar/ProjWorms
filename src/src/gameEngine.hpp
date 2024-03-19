#pragma once

// Inclure la bibliothèque SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h> 
#include <c3ga/Mvec.hpp> 

enum State {
    P1_MOVEMENT,
    P1_SHOOTING,
    P2_MOVEMENT,
    P2_SHOOTING,
    END_GAME,
};

typedef struct player {
    int x;
    int y;
} Player;

typedef struct rocket {
    int x;
    int y;
} Rocket;

void eventKeyDown(SDL_Event events);