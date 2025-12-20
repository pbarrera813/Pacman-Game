// GhostAI.h
// IA de los fantasmas del Pac-Man original
#pragma once

#include "Ghost.h"
#include "Pacman.h"
#include "Math.h"

class GhostAI {
public:
    // Calcula el objetivo para cada fantasma según su tipo y modo
    static Vector2 getTarget(
        const Ghost& ghost,
        const PacMan& pacman,
        const Ghost* blinky = nullptr  // Necesario para Inky
    );
    
private:
    // Objetivos en modo Chase
    static Vector2 getBlinkyTarget(const PacMan& pacman);
    static Vector2 getPinkyTarget(const PacMan& pacman);
    static Vector2 getInkyTarget(const PacMan& pacman, const Ghost* blinky);
    static Vector2 getClydeTarget(const Ghost& clyde, const PacMan& pacman);
    
    // Objetivos en modo Scatter (esquinas)
    static Vector2 getScatterTarget(GhostType type);
};
