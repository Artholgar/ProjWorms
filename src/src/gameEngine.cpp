#include "gameEngine.hpp"

void eventKeyDown(SDL_Event events) {
    if (events.key.keysym.scancode == SDL_SCANCODE_W) // Regarde si le scancode W est enfoncé (Z sous un azerty)
        SDL_Log("Scancode W"); // Affiche un message
}