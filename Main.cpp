// Main.cpp
// Punto de entrada del juego Pac-Man
#include "Game.h"
#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    
    Game game;
    
    if (!game.init()) {
        std::cerr << "Failed to initialize game" << std::endl;
        return -1;
    }
    
    // Game loop
    Uint32 lastTicks = SDL_GetTicks();
    const float targetFrameTime = 1.0f / 60.0f;  // 60 FPS
    
    while (game.isRunning()) {
        Uint32 currentTicks = SDL_GetTicks();
        float deltaTime = (currentTicks - lastTicks) / 1000.0f;
        lastTicks = currentTicks;
        
        // Limitar deltaTime para evitar problemas
        if (deltaTime > 0.1f) {
            deltaTime = 0.1f;
        }
        
        game.handleInput();
        game.update(deltaTime);
        game.render();
        
        // Frame rate limiting
        Uint32 frameTime = SDL_GetTicks() - currentTicks;
        if (frameTime < 16) {
            SDL_Delay(16 - frameTime);
        }
    }
    
    return 0;
}
