// Pacman.h
// Clase del jugador Pac-Man
#pragma once

#include "Entity.h"
#include "Direction.h"
#include <string>

class PacMan : public Entity {
public:
    PacMan();
    
    void update(float dt) override;
    void reset() override;
    
    // Control
    void setDesiredDirection(Direction dir);
    
    // Estado
    bool isAlive() const { return alive; }
    bool isEating() const { return eating; }
    
    // Eventos del último frame
    bool ateDot = false;
    bool atePowerPellet = false;
    bool ateFruit = false;
    
    // Animación de muerte
    void die();
    void updateDeathAnimation(float dt);
    bool isDeathAnimationComplete() const { return deathAnimComplete; }
    int getDeathFrame() const { return deathFrame; }
    
    // Obtener frame de animación actual
    int getAnimFrame() const { return animFrame; }
    
private:
    Direction desiredDirection = Direction::Left;
    
    bool alive = true;
    bool eating = false;
    
    // Animación normal (5 frames: boca cerrada -> abierta -> cerrada)
    float animTimer = 0.0f;
    int animFrame = 0;  // 0-4
    
    // Animación de muerte
    bool dying = false;
    bool deathAnimComplete = false;
    float deathTimer = 0.0f;
    int deathFrame = 0;
    
    // Movimiento
    bool canMove(Direction dir) const;
    void handleTunnelWrap();
};
