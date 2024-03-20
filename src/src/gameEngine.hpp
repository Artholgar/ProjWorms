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
    bool isDead = false;
    Vec2d forces = {0, 0};
    double canonAngle = 0.;

    bool operator==(const Player& other) const {
        return (x == other.x && y == other.y);
    }

    bool operator!=(const Player& other) const {
        return (x != other.x || y != other.y);
    }

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
            while(_players[_currentPlayerIdx].isDead) {
                _currentPlayerIdx += 1;
                _currentPlayerIdx = _currentPlayerIdx % _nbPlayer;
            }

            auto& player = _players[_currentPlayerIdx];
            return player;
        }

        void nextRound() {
            _state = MOVEMENT;
            _countMovement = 0;
            _round += 1;
            _currentPlayerIdx += 1;
            _currentPlayerIdx = _currentPlayerIdx % _nbPlayer;
        }

        std::vector<Player> getPlayers() {
            return _players;
        }

        void drawWorms(SDL_Renderer* pRenderer, SDL_Texture* pTexture) {
            SDL_SetRenderDrawColor(pRenderer, 0, 0, 255, 255);

            SDL_RenderDrawLine(pRenderer, _ground.pt1[c3ga::E1], _ground.pt1[c3ga::E2], _ground.pt2[c3ga::E1], _ground.pt2[c3ga::E2]);

            for (const auto player : _players) {
                if (player.isDead) {
                    continue;
                }
                SDL_Rect rect{(int)player.x - 25, (int)player.y - 25, 50, 50};
                SDL_RenderCopy(pRenderer, pTexture, nullptr, &rect);
            }

            auto player = getCurrentPlayer();
            SDL_RenderDrawLine(pRenderer, player.x, player.y, player.x + 20 * cos(player.canonAngle), player.y + 20 * sin(player.canonAngle));
            SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
            
            for (const auto rocket : _rockets) {
                SDL_Rect rect{(int)rocket.x - 10, (int)rocket.y - 10, 20, 20};
                SDL_RenderDrawRect(pRenderer, &rect);
            }
        }

        void updatePlayers() {
            for (auto& player : _players) {
                // represent the player as a circle
                c3ga::Mvec<double> playerC = c3ga::point<double>(player.x, player.y + 25,-1)
                         ^ c3ga::point<double>(player.x + 25, player.y,0)
                         ^ c3ga::point<double>(player.x - 25, player.y,1);

                if ((player.x <= _width || player.forces.x <= 0) && (player.x >= 0 || player.forces.x >= 0)) {
                    player.x += player.forces.x;
                }

                // ground collision
                c3ga::Mvec<double> dual_circle = (!_ground.vec) | playerC;
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

                // represent the rocket as a circle
                c3ga::Mvec<double> rocketC = c3ga::point<double>(rocket.x, rocket.y - 10,-1)
                    ^ c3ga::point<double>(rocket.x + 10, rocket.y,0)
                    ^ c3ga::point<double>(rocket.x - 10, rocket.y,1);

                // ground collision
                c3ga::Mvec<double> dual_circle = (!_ground.vec) | rocketC;
                double r2 = dual_circle | dual_circle;

                if (r2 < 0) {
                    _rockets.erase(_rockets.begin());
                    _isRocket = false;
                    nextRound();
                    return;
                }

                // collision player rocket
                for (auto it = _players.begin(); it != _players.end(); ++it) {
                    auto& player = *it;

                    auto dist = sqrt(pow(rocket.x - player.x, 2) + pow(rocket.y - player.y, 2));

                    if (dist <= 25 + 10 && player != getCurrentPlayer()) {
                        _rockets.erase(_rockets.begin());
                        player.isDead = true;
                        _isRocket = false;
                        nextRound();
                        return;
                    }
                }
            }
        }

        void eventKeyDown(SDL_Event events) {
            if (_isRocket) {
                return;
            }
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
                if (events.key.keysym.scancode == SDL_SCANCODE_RETURN && !_isRocket) {
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
                    _rockets.emplace_back(Rocket(player.x, player.y, cos(player.canonAngle), sin(player.canonAngle)));
                    _isRocket = true;
                    return;
                }
            }   
        }

    private:
        void initializePlayer()
        {
            _ground.pt1 = c3ga::point<double>(0, _height - 150, 0);
            _ground.pt2 = c3ga::point<double>(_width, _height - 150, 0);

            _ground.vec = _ground.pt1 ^ _ground.pt2 ^ c3ga::ei<double>();

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
        int _maxMovement = 10;
        int _g = 1;
        Ground _ground;
        std::vector<Rocket> _rockets;
        bool _isRocket = false;
        int _currentPlayerIdx = 0;
};