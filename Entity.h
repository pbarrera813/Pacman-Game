// Entity.h
// Clase base para Pac-Man y Fantasmas
#pragma once

#include "Math.h"
#include "Direction.h"
#include "Constants.h"

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
    
    // Verificar si está centrado en un tile
    bool isCentered() const {
        int px = static_cast<int>(position.x / SCALE);
        int py = static_cast<int>(position.y / SCALE);
        return (px % TILE_SIZE == 0) && (py % TILE_SIZE == 0);
    }
};
