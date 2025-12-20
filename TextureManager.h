// TextureManager.h
// Gestor de texturas SDL2
#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <unordered_map>

class TextureManager {
public:
    static TextureManager& get();
    
    bool init(SDL_Renderer* renderer);
    void shutdown();
    
    bool load(const std::string& id, const std::string& path);
    SDL_Texture* getTexture(const std::string& id);
    
    // Dibujar textura
    void draw(const std::string& id, int x, int y, int w, int h, 
              double angle = 0.0, SDL_RendererFlip flip = SDL_FLIP_NONE);
    
    // Dibujar con source rect
    void drawFrame(const std::string& id, int x, int y, int w, int h,
                   int frameX, int frameY, int frameW, int frameH);
    
    void clear();
    
private:
    TextureManager() = default;
    ~TextureManager() = default;
    
    SDL_Renderer* renderer = nullptr;
    std::unordered_map<std::string, SDL_Texture*> textures;
};
