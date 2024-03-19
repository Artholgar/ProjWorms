#pragma once

// Inclure la bibliothèque SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h> 
#include <c3ga/Mvec.hpp> 

#include "c3gaTools.hpp"

#include <cmath>

enum State {
    MOVEMENT,
    SHOOTING,
    END_GAME,
};

typedef struct vec2d {
    int x;
    int y;
} Vec2d;

class Player {
    public:
    int x;
    int y;
    Vec2d forces = {0, 0};
    double canonAngle = 0.;

    Player(int x, int y) : x{x}, y{y}{}
};

typedef struct rocket {
    int x;
    int y;
} Rocket;

typedef struct ground {
    c3ga::Mvec<double> pt1;
    c3ga::Mvec<double> pt2;
    c3ga::Mvec<double> vec;
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

        Player& getCurrentPlayer() {
            return _players[_round % _nbPlayer];
        }

        void nextRound() {
            _state = MOVEMENT;
            _countMovement = 0;
            _round += 1;
        }

        std::vector<Player> getPlayers() {
            return _players;
        }

        void drawWorms(SDL_Renderer* pRenderer, SDL_Texture* pTexture) {

            for (const auto player : _players) {
                SDL_Rect rect{player.x - 25, player.y - 25, 50, 50};
                SDL_RenderCopy(pRenderer, pTexture, nullptr, &rect);
                // SDL_RenderDrawRect(pRenderer, &rect);
            }
            auto player = getCurrentPlayer();
            SDL_RenderDrawLine(pRenderer, player.x, player.y, player.x + 20 * cos(player.canonAngle), player.y + 20 * sin(player.canonAngle));
        }

        void updatePlayers() {
            for (auto& player : _players) {
                c3ga::Mvec<double> playerC = c3ga::point<double>(player.x, player.y + 10,0)
                         ^ c3ga::point<double>(player.x + 3, player.y - 7,0)
                         ^ c3ga::point<double>(player.x - 3, player.y - 7,0);

                player.x += player.forces.x;

                c3ga::Mvec<double> res = !( !playerC ^ !ground.vec );

                c3ga::Mvec<double> dual_circle = (!ground.vec) | playerC;
                double r2 = dual_circle | dual_circle;

                if (r2 < 0) {
                    player.y += player.forces.y; 
                }

                

                player.forces.x = 0;
                player.forces.y = _g; // gavity
            }
        }

        void eventKeyDown(SDL_Event events) {
            if (_state == MOVEMENT) {
                if (events.key.keysym.scancode == SDL_SCANCODE_D) { // Regarde si le scancode W est enfoncé (Z sous un azerty)
                    auto& player = getCurrentPlayer();
                    
                    if (player.x <= _width) {
                        player.x += 1;
                        _countMovement += 1;
                    }
                }
                if (events.key.keysym.scancode == SDL_SCANCODE_A) { // Regarde si le scancode W est enfoncé (Z sous un azerty)
                    auto& player = getCurrentPlayer();

                    if (player.x >= 0) {
                        player.x -= 1;
                        _countMovement += 1;
                    }
                }

                if (events.key.keysym.scancode == SDL_SCANCODE_RETURN || _countMovement >= _maxMovement) {
                    _state = SHOOTING;
                    return;
                }
            }
            if (_state == SHOOTING) {
                if (events.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                    nextRound();
                    return;
                }
                if (events.key.keysym.scancode == SDL_SCANCODE_D) { // Regarde si le scancode W est enfoncé (Z sous un azerty)
                    auto& player = getCurrentPlayer();
                              
                    player.canonAngle += M_PI / 180.;
                }
                if (events.key.keysym.scancode == SDL_SCANCODE_A) { // Regarde si le scancode W est enfoncé (Z sous un azerty)
                    auto& player = getCurrentPlayer();

                    player.canonAngle -= M_PI / 180.;
                }
            }   
        }

    private:
        void initializePlayer()
        {
            ground.pt1 = c3ga::point<double>(0, _height - 150, 0);
            ground.pt2 = c3ga::point<double>(_width, _height - 150, 0);

            ground.vec = ground.pt1 ^ ground.pt2;

            for(unsigned int i = 0; i < _nbPlayer; i++)
            {
                _players.emplace_back(Player((_width / _nbPlayer) * i + (_width / _nbPlayer) / 2, _height - 200));
            }
        }
        unsigned int _nbPlayer;
        int _height;
        int _width;
        std::vector<Player> _players;
        int _round = 0;
        State _state = MOVEMENT;
        int _countMovement = 0;
        int _maxMovement = 50;
        int _g = 1;
        Ground ground;
};