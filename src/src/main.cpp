// Inclure la bibliothèque SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h> 
#include <c3ga/Mvec.hpp> 

// Inclusion des bibliothèque standard C++ 
#include <cstdlib>   

#include "gameEngine.hpp"

// Définition des constante 
template<typename T>
constexpr T WIDTHSCREEN{ 800 };

template<typename T>
constexpr T HEIGHTSCREEN{ 600 };

int main(int argc, char* argv[])
{
    // Chargement du module vidéo de la SDL 
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[DEBUG] > %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Ressource : Fenêtre et rendu 
    SDL_Window* pWindow{ nullptr };
    SDL_Renderer* pRenderer{ nullptr };

    // Création d'une fenêtre, en 800x600, et visible
    if (SDL_CreateWindowAndRenderer(WIDTHSCREEN<unsigned int>, HEIGHTSCREEN<unsigned int>, SDL_WINDOW_SHOWN, &pWindow, &pRenderer) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[DEBUG] > %s", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_RendererInfo infoRenderer;
    SDL_GetRendererInfo(pRenderer, &infoRenderer);

    if (infoRenderer.flags & SDL_RENDERER_ACCELERATED)
    {
        SDL_Log("Le rendu est gérer par la carte graphique...");

    }
    if (infoRenderer.flags & SDL_RENDERER_SOFTWARE)
    {
        SDL_Log("Le rendu est gérer par la carte graphique...");
    }

    if (infoRenderer.flags & SDL_RENDERER_TARGETTEXTURE)
    {
        SDL_Log("Le rendu est autoriser sur des texture...");
    }

    // Load textures
    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) )
    {
        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
        return EXIT_FAILURE;
    }

    SDL_Surface *pSurface {nullptr};
    pSurface=IMG_Load("../textures/player.png");
    if (NULL == pSurface) {
        printf("SDL surface could not be loaded\n");
        return EXIT_FAILURE;
    }
    SDL_Texture *pTexture=SDL_CreateTextureFromSurface(pRenderer,pSurface);

    //SDL_Texture* pTexture = IMG_LoadTexture(pRenderer, "../../textures/player.png");

    SDL_Event events;
    bool isOpen{ true };

    SDL_Rect rectangle1{WIDTHSCREEN<int> / 2 - 100 / 2, HEIGHTSCREEN<int> / 2 - 50 / 2, 100, 50};


    // Game loop
    while (isOpen)
    {
        // Input 
        while (SDL_PollEvent(&events))
        {
            switch (events.type)
            {
            case SDL_QUIT:
                isOpen = false;
                break;
            }
        }


        // Rendering
        SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255); // Choisir la couleur noir  
        SDL_RenderClear(pRenderer); // Colorier en noir toutes la fenêtre 
        // SDL_SetRenderDrawColor(pRenderer, 0, 0, 255, 255);
        SDL_RenderCopy(pRenderer, pTexture, nullptr, nullptr);
        // SDL_RenderDrawRect(pRenderer, &rectangle1);
        // draw_rectangle_not_fill(pRenderer, rectangle1, SDL_Color{ 0, 0, 255, 255 });

        SDL_RenderPresent(pRenderer); // Mise à jour de la fenêtre 
    }

    // Libération des ressource en mémoire
    SDL_DestroyTexture(pTexture);
    IMG_Quit();
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();

    return EXIT_SUCCESS;
}