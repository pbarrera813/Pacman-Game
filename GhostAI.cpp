// GhostAI.cpp
// IA fiel al Pac-Man arcade original
#include "GhostAI.h"
#include "Constants.h"
#include <cmath>

// Helper: convertir tile a posición en píxeles
static Vector2 tileToPixel(int x, int y) {
    return Vector2(
        static_cast<float>(x * TILE_SIZE * SCALE),
        static_cast<float>(y * TILE_SIZE * SCALE)
    );
}

Vector2 GhostAI::getTarget(const Ghost& ghost, const PacMan& pacman, const Ghost* blinky) {
    GhostMode mode = ghost.getMode();
    
    // Modo Eyes: ir a la casa
    if (mode == GhostMode::Eyes) {
        return tileToPixel(GHOST_HOUSE_X, 11);
    }
    
    // Modo Frightened: movimiento aleatorio (ia de fantasmas invertida para esto)
    if (mode == GhostMode::Frightened) {
        // En el original es aleatorio, pero para simplificar huyen de Pac-Man
        return Vector2(pacman.position.x, pacman.position.y);
    }
    
    // Modo Scatter: ir a esquina asignada
    if (mode == GhostMode::Scatter) {
        return getScatterTarget(ghost.getType());
    }
    
    // Modo Chase: según el tipo de fantasma
    switch (ghost.getType()) {
        case GhostType::Blinky:
            return getBlinkyTarget(pacman);
        case GhostType::Pinky:
            return getPinkyTarget(pacman);
        case GhostType::Inky:
            return getInkyTarget(pacman, blinky);
        case GhostType::Clyde:
            return getClydeTarget(ghost, pacman);
        default:
            return getBlinkyTarget(pacman);
    }
}

// BLINKY (Rojo) - "Shadow"
// Persigue directamente a Pac-Man
Vector2 GhostAI::getBlinkyTarget(const PacMan& pacman) {
    return Vector2(pacman.position.x, pacman.position.y);
}

// PINKY (Rosa) - "Speedy"
// Apunta 4 tiles delante de Pac-Man
Vector2 GhostAI::getPinkyTarget(const PacMan& pacman) {
    int px = pacman.getTileX();
    int py = pacman.getTileY();
    
    // 4 tiles delante
    switch (pacman.direction) {
        case Direction::Up:
            py -= 4;
            // Bug del arcade original: también se mueve 4 a la izquierda cuando va arriba
            px -= 4;
            break;
        case Direction::Down:
            py += 4;
            break;
        case Direction::Left:
            px -= 4;
            break;
        case Direction::Right:
            px += 4;
            break;
        default:
            break;
    }
    
    return tileToPixel(px, py);
}

// INKY (Cyan) - "Bashful"
// Emboscada compleja: dibuja vector desde Blinky a 2 tiles delante de Pac-Man,
// luego duplica ese vector
Vector2 GhostAI::getInkyTarget(const PacMan& pacman, const Ghost* blinky) {
    if (!blinky) {
        return getBlinkyTarget(pacman);
    }
    
    int px = pacman.getTileX();
    int py = pacman.getTileY();
    
    // 2 tiles delante de Pac-Man
    switch (pacman.direction) {
        case Direction::Up:
            py -= 2;
            px -= 2;  // Bug del original
            break;
        case Direction::Down:
            py += 2;
            break;
        case Direction::Left:
            px -= 2;
            break;
        case Direction::Right:
            px += 2;
            break;
        default:
            break;
    }
    
    // Vector desde Blinky al punto delante de Pac-Man
    int bx = blinky->getTileX();
    int by = blinky->getTileY();
    
    int vx = px - bx;
    int vy = py - by;
    
    // Duplicar el vector
    int targetX = px + vx;
    int targetY = py + vy;
    
    return tileToPixel(targetX, targetY);
}

// CLYDE (Naranja) - "Pokey"
// Si está a más de 8 tiles de Pac-Man: perseguir como Blinky
// Si está a 8 tiles o menos: ir a su esquina de scatter
Vector2 GhostAI::getClydeTarget(const Ghost& clyde, const PacMan& pacman) {
    int gx = clyde.getTileX();
    int gy = clyde.getTileY();
    int px = pacman.getTileX();
    int py = pacman.getTileY();
    
    // Distancia en tiles
    float dist = std::sqrt(
        static_cast<float>((gx - px) * (gx - px) + (gy - py) * (gy - py))
    );
    
    if (dist > 8) {
        // Perseguir como Blinky
        return Vector2(pacman.position.x, pacman.position.y);
    }
    else {
        // Ir a esquina
        return getScatterTarget(GhostType::Clyde);
    }
}

// Esquinas de scatter para cada fantasma
Vector2 GhostAI::getScatterTarget(GhostType type) {
    switch (type) {
        case GhostType::Blinky:
            return tileToPixel(BLINKY_SCATTER_X, BLINKY_SCATTER_Y);
        case GhostType::Pinky:
            return tileToPixel(PINKY_SCATTER_X, PINKY_SCATTER_Y);
        case GhostType::Inky:
            return tileToPixel(INKY_SCATTER_X, INKY_SCATTER_Y);
        case GhostType::Clyde:
            return tileToPixel(CLYDE_SCATTER_X, CLYDE_SCATTER_Y);
        default:
            return tileToPixel(MAP_WIDTH / 2, MAP_HEIGHT / 2);
    }
}
