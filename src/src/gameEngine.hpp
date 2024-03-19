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
    double x;
    double y;
} Vec2d;

class Player {
    public:
    double x;
    double y;
    Vec2d forces = {0, 0};
    double canonAngle = 0.;

    Player(double x, double y) : x{x}, y{y}{}
};

class Rocket {
    public:
    double x;
    double y;
    Vec2d forces;

    Rocket(double x, double y, double forceX, double forceY) : x{x}, y{y}, forces{forceX, forceY} {}
};

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
            SDL_SetRenderDrawColor(pRenderer, 0, 0, 255, 255);
            for (const auto player : _players) {
                SDL_Rect rect{(int)player.x - 25, (int)player.y - 25, 50, 50};
                SDL_RenderCopy(pRenderer, pTexture, nullptr, &rect);
                // SDL_RenderDrawRect(pRenderer, &rect);
            }
            auto player = getCurrentPlayer();
            SDL_RenderDrawLine(pRenderer, player.x, player.y, player.x + 20 * cos(player.canonAngle), player.y + 20 * sin(player.canonAngle));
            SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
            for (const auto rocket : _rockets) {
                //std::cout << rocket.x << " " << rocket.y << std::endl;
                SDL_Rect rect{(int)rocket.x - 10, (int)rocket.y - 10, 20, 20};
                SDL_RenderDrawRect(pRenderer, &rect);
            }
        }

        void updatePlayers() {
            for (auto& player : _players) {
                // represent the player as a circle
                c3ga::Mvec<double> playerC = c3ga::point<double>(player.x, player.y + 10,-1)
                         ^ c3ga::point<double>(player.x + 3, player.y - 7,0)
                         ^ c3ga::point<double>(player.x - 3, player.y - 7,1);

                if (player.x <= _width && player.x >= 0) {
                    player.x += player.forces.x;
                }

                // ground collision
                c3ga::Mvec<double> res = !( !playerC ^ !ground.vec );

                c3ga::Mvec<double> dual_circle = (!ground.vec) | playerC;
                double r2 = dual_circle | dual_circle;

                if (r2 >= 0) {
                    player.y += player.forces.y; 
                }

                

                player.forces.x -= player.forces.x * 0.1;
                player.forces.y -= player.forces.y * 0.1 - _g * 0.1;
            }

            for (auto& rocket : _rockets) {
                rocket.x += rocket.forces.x;
                rocket.y += rocket.forces.y;
                
                rocket.forces.x -= rocket.forces.x * 0.001;
                rocket.forces.y -= rocket.forces.y * 0.001 - _g * 0.001;
            }
        }

        void eventKeyDown(SDL_Event events) {
            if (_state == MOVEMENT) {
                if (events.key.keysym.scancode == SDL_SCANCODE_D) {
                    auto& player = getCurrentPlayer();
                    
                    player.forces.x = 1;
                    _countMovement += 1;
                    
                }
                if (events.key.keysym.scancode == SDL_SCANCODE_A) {
                    auto& player = getCurrentPlayer();

                    player.forces.x = -1;
                    _countMovement += 1;
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
                if (events.key.keysym.scancode == SDL_SCANCODE_D) {
                    auto& player = getCurrentPlayer();
                              
                    player.canonAngle += M_PI / 180.;
                }
                if (events.key.keysym.scancode == SDL_SCANCODE_A) {
                    auto& player = getCurrentPlayer();

                    player.canonAngle -= M_PI / 180.;
                }
                if (events.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                    auto& player = getCurrentPlayer();
                    std::cout << cos(player.canonAngle) << " " << sin(player.canonAngle) << std::endl;
                    _rockets.emplace_back(Rocket(player.x, player.y, cos(player.canonAngle), sin(player.canonAngle)));
                    nextRound();
                    return;
                }
            }   
        }

    private:
        void initializePlayer()
        {
            ground.pt1 = c3ga::point<double>(0, _height - 150, 0);
            ground.pt2 = c3ga::point<double>(_width, _height - 150, 0);

            ground.vec = ground.pt1 ^ ground.pt2 ^ c3ga::ei<double>();

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
        std::vector<Rocket> _rockets;
};