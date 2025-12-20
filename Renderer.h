// Renderer.h
// Sistema de renderizado
#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    bool init();
    void shutdown();
    
    // Frame
    void clear();
    void present();
    
    // Dibujar
    void drawMaze();           // Dibujar laberinto con código
    void drawDots();
    void drawScore(int score, int highScore, int lives);
    void drawText(const std::string& textureId, int x, int y);
    void drawLives(int lives);
    void drawFruit();
    
    // Acceso al renderer SDL
    SDL_Renderer* getSDLRenderer() const { return renderer; }
    
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;
    
    void drawNumber(int number, int x, int y);
    void drawWallTile(int x, int y);
};
