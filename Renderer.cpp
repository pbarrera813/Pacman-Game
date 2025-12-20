// Renderer.cpp
#include "Renderer.h"
#include "TextureManager.h"
#include "Map.h"
#include "Constants.h"
#include <iostream>
#include <string>

Renderer::Renderer() {}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL init failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    if (TTF_Init() < 0) {
        std::cerr << "TTF init failed: " << TTF_GetError() << std::endl;
        return false;
    }
    
    window = SDL_CreateWindow(
        "PAC-MAN",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    font = TTF_OpenFont("assets/fonts/arcade_n.ttf", 8 * SCALE);
    if (!font) {
        std::cerr << "Font loading failed: " << TTF_GetError() << std::endl;
    }
    
    return true;
}

void Renderer::shutdown() {
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    
    TextureManager::get().shutdown();
    
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    TTF_Quit();
    SDL_Quit();
}

void Renderer::clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Renderer::present() {
    SDL_RenderPresent(renderer);
}

void Renderer::drawWallTile(int tileX, int tileY) {
    SDL_SetRenderDrawColor(renderer, 33, 33, 222, 255);
    
    int x = tileX * SCALED_TILE;
    int y = tileY * SCALED_TILE + GAME_OFFSET_Y;
    
    bool wallUp = Map::get().getTile(tileX, tileY - 1) == TileType::Wall;
    bool wallDown = Map::get().getTile(tileX, tileY + 1) == TileType::Wall;
    bool wallLeft = Map::get().getTile(tileX - 1, tileY) == TileType::Wall;
    bool wallRight = Map::get().getTile(tileX + 1, tileY) == TileType::Wall;
    
    int border = 2 * SCALE;
    
    if (!wallUp) {
        SDL_Rect rect = {x, y, SCALED_TILE, border};
        SDL_RenderFillRect(renderer, &rect);
    }
    if (!wallDown) {
        SDL_Rect rect = {x, y + SCALED_TILE - border, SCALED_TILE, border};
        SDL_RenderFillRect(renderer, &rect);
    }
    if (!wallLeft) {
        SDL_Rect rect = {x, y, border, SCALED_TILE};
        SDL_RenderFillRect(renderer, &rect);
    }
    if (!wallRight) {
        SDL_Rect rect = {x + SCALED_TILE - border, y, border, SCALED_TILE};
        SDL_RenderFillRect(renderer, &rect);
    }
}

void Renderer::drawMaze() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            TileType tile = Map::get().getTile(x, y);
            
            if (tile == TileType::Wall) {
                drawWallTile(x, y);
            }
            else if (tile == TileType::GhostDoor) {
                SDL_SetRenderDrawColor(renderer, 255, 184, 222, 255);
                SDL_Rect rect = {
                    x * SCALED_TILE,
                    y * SCALED_TILE + GAME_OFFSET_Y + SCALED_TILE / 3,
                    SCALED_TILE,
                    SCALED_TILE / 3
                };
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

void Renderer::drawDots() {
    auto& tm = TextureManager::get();
    
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            TileType tile = Map::get().getTile(x, y);
            
            int px = x * SCALED_TILE;
            int py = y * SCALED_TILE + GAME_OFFSET_Y;
            
            if (tile == TileType::Dot) {
                // Dot normal: pequeño y centrado (6x6 píxeles)
                int dotSize = 2 * SCALE;  // 6 píxeles
                int offset = (SCALED_TILE - dotSize) / 2;
                tm.draw("pill", px + offset, py + offset, dotSize, dotSize);
            }
            else if (tile == TileType::PowerPellet) {
                // Power pellet: más grande pero no todo el tile (18x18 píxeles)
                int pelletSize = 6 * SCALE;  // 18 píxeles
                int offset = (SCALED_TILE - pelletSize) / 2;
                tm.draw("super_pill", px + offset, py + offset, pelletSize, pelletSize);
            }
        }
    }
}

void Renderer::drawNumber(int number, int x, int y) {
    if (!font) return;
    
    std::string text = std::to_string(number);
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), white);
    if (!surface) return;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture) {
        SDL_Rect dst = {x, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
}

void Renderer::drawScore(int score, int highScore, int /*lives*/) {
    if (!font) return;
    
    SDL_Color white = {255, 255, 255, 255};
    
    // "1UP"
    SDL_Surface* surf1 = TTF_RenderText_Solid(font, "1UP", white);
    if (surf1) {
        SDL_Texture* tex1 = SDL_CreateTextureFromSurface(renderer, surf1);
        if (tex1) {
            SDL_Rect dst = {3 * SCALED_TILE, 0, surf1->w, surf1->h};
            SDL_RenderCopy(renderer, tex1, nullptr, &dst);
            SDL_DestroyTexture(tex1);
        }
        SDL_FreeSurface(surf1);
    }
    
    drawNumber(score, 1 * SCALED_TILE, 8 * SCALE);
    
    // "HIGH SCORE"
    SDL_Surface* surf2 = TTF_RenderText_Solid(font, "HIGH SCORE", white);
    if (surf2) {
        SDL_Texture* tex2 = SDL_CreateTextureFromSurface(renderer, surf2);
        if (tex2) {
            SDL_Rect dst = {9 * SCALED_TILE, 0, surf2->w, surf2->h};
            SDL_RenderCopy(renderer, tex2, nullptr, &dst);
            SDL_DestroyTexture(tex2);
        }
        SDL_FreeSurface(surf2);
    }
    
    drawNumber(highScore, 13 * SCALED_TILE, 8 * SCALE);
}

void Renderer::drawLives(int lives) {
    auto& tm = TextureManager::get();
    
    int y = SCREEN_HEIGHT - 2 * SCALED_TILE;
    
    for (int i = 0; i < lives - 1; i++) {
        int x = (2 + i * 2) * SCALED_TILE;
        tm.draw("pacman_life", x, y, SCALED_TILE * 13 / 8, SCALED_TILE * 13 / 8);
    }
}

void Renderer::drawFruit() {
    auto& tm = TextureManager::get();
    tm.draw("cherry", SCREEN_WIDTH - 3 * SCALED_TILE, 
            SCREEN_HEIGHT - 2 * SCALED_TILE, 
            SCALED_TILE * 13 / 8, SCALED_TILE * 13 / 8);
}

void Renderer::drawText(const std::string& textureId, int x, int y) {
    auto& tm = TextureManager::get();
    SDL_Texture* tex = tm.getTexture(textureId);
    
    if (tex) {
        int w, h;
        SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
        tm.draw(textureId, x, y, w * SCALE, h * SCALE);
    }
}