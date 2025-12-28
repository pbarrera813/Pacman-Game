// Pacman.cpp
#include "Pacman.h"
#include "Map.h"
#include "Constants.h"

PacMan::PacMan() {
    reset();
}

void PacMan::reset() {
    // Posición inicial (centro de tile, escalada)
    position.x = PACMAN_START_X * TILE_SIZE * SCALE;
    position.y = PACMAN_START_Y * TILE_SIZE * SCALE;
    
    direction = Direction::Left;
    desiredDirection = Direction::Left;
    speed = PACMAN_SPEED;
    speedMultiplier = 1.0f;
    
    alive = true;
    eating = false;
    dying = false;
    deathAnimComplete = false;
    
    ateDot = false;
    atePowerPellet = false;
    ateFruit = false;
    
    animTimer = 0.0f;
    animFrame = 0;
    deathTimer = 0.0f;
    deathFrame = 0;
}

void PacMan::setDesiredDirection(Direction dir) {
    desiredDirection = dir;
}

bool PacMan::canMove(Direction dir) const {
    int tx = getTileX();
    int ty = getTileY();
    
    switch (dir) {
        case Direction::Up:    ty--; break;
        case Direction::Down:  ty++; break;
        case Direction::Left:  tx--; break;
        case Direction::Right: tx++; break;
        default: return false;
    }
    
    return Map::get().isWalkable(tx, ty, false);
}

void PacMan::handleTunnelWrap() {
    // Túnel izquierdo
    if (position.x < -SCALED_TILE) {
        position.x = MAP_WIDTH * SCALED_TILE;
    }
    // Túnel derecho
    else if (position.x > MAP_WIDTH * SCALED_TILE) {
        position.x = -SCALED_TILE;
    }
}

void PacMan::update(float dt) {
    if (!alive || dying)
        return;
    
    // Reset eventos
    ateDot = false;
    atePowerPellet = false;
    ateFruit = false;
    eating = false;
    
    // Solo cambiar dirección cuando está centrado en un tile
    if (isCentered()) {
        // Intentar cambiar a la dirección deseada
        if (canMove(desiredDirection)) {
            direction = desiredDirection;
        }
        // Si no puede continuar en la dirección actual, detenerse
        else if (!canMove(direction)) {
            direction = Direction::None;
        }
        
        // Comer dot o power pellet
        int tx = getTileX();
        int ty = getTileY();
        
        if (Map::get().eatDot(tx, ty)) {
            ateDot = true;
            eating = true;
            // El sonido waka se maneja en Game.cpp
        }
        else if (Map::get().eatPowerPellet(tx, ty)) {
            atePowerPellet = true;
            eating = true;
        }
    }
    
    // Movimiento
    if (direction != Direction::None) {
        float moveSpeed = eating ? PACMAN_DOT_SPEED : speed;
        moveSpeed *= speedMultiplier;  // Aplicar multiplicador de nivel
        float move = moveSpeed * dt;
        
        switch (direction) {
            case Direction::Up:    position.y -= move; break;
            case Direction::Down:  position.y += move; break;
            case Direction::Left:  position.x -= move; break;
            case Direction::Right: position.x += move; break;
            default: break;
        }
        
        // Animación waka-waka (solo si se mueve)
        animTimer += dt;
        if (animTimer >= PACMAN_ANIM_SPEED) {
            animTimer = 0.0f;
            animFrame = (animFrame + 1) % 5;  // 5 frames de animación
        }
    }
    else {
        // Si está quieto, boca cerrada (frame 0)
        animFrame = 0;
    }
    
    // Wrap en túneles
    handleTunnelWrap();
}

void PacMan::die() {
    alive = false;
    dying = true;
    deathTimer = 0.0f;
    deathFrame = 0;
    deathAnimComplete = false;
    direction = Direction::None;
}

void PacMan::updateDeathAnimation(float dt) {
    if (!dying)
        return;
    
    deathTimer += dt;
    
    // 12 frames de animación de muerte (0.1s por frame = 1.2s total)
    if (deathTimer >= 0.1f) {
        deathTimer = 0.0f;
        deathFrame++;
        
        if (deathFrame >= 12) {
            deathAnimComplete = true;
            dying = false;
        }
    }
}