#pragma once

// Inclure la bibliothèque SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h> 
#include <c3ga/Mvec.hpp> 

#include "c3gaTools.hpp"

enum State {
    P1_MOVEMENT,
    P1_SHOOTING,
    P2_MOVEMENT,
    P2_SHOOTING,
    END_GAME,
};

class Player {
    public:
    int x;
    int y;
    Player(int x, int y) : x{x}, y{y}{}
};

typedef struct rocket {
    int x;
    int y;
} Rocket;

typedef struct ground {
    c3ga::Mvec<double> ground;
} Ground;

class GameEngine
{
    public:
        GameEngine(unsigned int nbPlayer, int height, int width)
        : _nbPlayer {nbPlayer}
        , _height {height}
        , _width {width}
        {
            initializePlayer();
        }

        Player* getCurrentPlayer() {
            return &_players[_round % _nbPlayer];
        }

        void nextRound() {
            _round += 1;
        }

        std::vector<Player> getPlayers() {
            return _players;
        }

    private:
        void initializePlayer()
        {
            for(unsigned int i = 0; i < _nbPlayer; i++)
            {
                _players.emplace_back(Player((_width / _nbPlayer) * i + (_width / _nbPlayer) / 2, _height - 200));
            }
        }
        unsigned int _nbPlayer;
        int _height;
        int _width;
        std::vector<Player> _players;
        int _round;
};


void eventKeyDown(SDL_Event events);