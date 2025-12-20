// TextureManager.cpp
#include "TextureManager.h"
#include <iostream>

TextureManager& TextureManager::get() {
    static TextureManager instance;
    return instance;
}

bool TextureManager::init(SDL_Renderer* r) {
    renderer = r;
    
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image init failed: " << IMG_GetError() << std::endl;
        return false;
    }
    
    return true;
}

void TextureManager::shutdown() {
    clear();
    IMG_Quit();
}

bool TextureManager::load(const std::string& id, const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "Failed to load image: " << path << " - " << IMG_GetError() << std::endl;
        return false;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (!texture) {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
        return false;
    }
    
    textures[id] = texture;
    return true;
}

SDL_Texture* TextureManager::getTexture(const std::string& id) {
    auto it = textures.find(id);
    return (it != textures.end()) ? it->second : nullptr;
}

void TextureManager::draw(const std::string& id, int x, int y, int w, int h,
                          double angle, SDL_RendererFlip flip) {
    SDL_Texture* tex = getTexture(id);
    if (!tex) return;
    
    SDL_Rect dst = {x, y, w, h};
    SDL_RenderCopyEx(renderer, tex, nullptr, &dst, angle, nullptr, flip);
}

void TextureManager::drawFrame(const std::string& id, int x, int y, int w, int h,
                               int frameX, int frameY, int frameW, int frameH) {
    SDL_Texture* tex = getTexture(id);
    if (!tex) return;
    
    SDL_Rect src = {frameX, frameY, frameW, frameH};
    SDL_Rect dst = {x, y, w, h};
    SDL_RenderCopy(renderer, tex, &src, &dst);
}

void TextureManager::clear() {
    for (auto& pair : textures) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
        }
    }
    textures.clear();
}
