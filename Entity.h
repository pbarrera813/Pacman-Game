// Entity.h
// Clase base para Pac-Man y Fantasmas
#pragma once

#include "Math.h"
#include "Direction.h"
#include "Constants.h"
#include <cmath>

class Entity {
public:
    virtual ~Entity() = default;
    
    virtual void update(float dt) = 0;
    virtual void reset() = 0;

    // Posición en píxeles (escalados)
    Vector2 position;
    
    // Dirección actual de movimiento
    Direction direction = Direction::None;
    
    // Velocidad actual
    float speed = 0.0f;
    
    // Obtener tile actual
    int getTileX() const {
        return static_cast<int>((position.x / SCALE + TILE_SIZE / 2) / TILE_SIZE);
    }
    
    int getTileY() const {
        return static_cast<int>((position.y / SCALE + TILE_SIZE / 2) / TILE_SIZE);
    }
    
    // Verificar si está centrado en un tile (con tolerancia)
    bool isCentered() const {
        float tileSize = static_cast<float>(TILE_SIZE * SCALE);
        float offsetX = std::fmod(position.x, tileSize);
        float offsetY = std::fmod(position.y, tileSize);
        
        // Tolerancia de 3 píxeles escalados
        float tolerance = 3.0f;
        
        bool centeredX = (offsetX <= tolerance) || (offsetX >= tileSize - tolerance);
        bool centeredY = (offsetY <= tolerance) || (offsetY >= tileSize - tolerance);
        
        return centeredX && centeredY;
    }
};
