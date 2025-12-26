// Ghost.h
// Fantasmas del Pac-Man
#pragma once

#include "Entity.h"
#include "Math.h"
#include "Constants.h"
#include "Direction.h"

#include <SDL2/SDL.h>
#include <string>

enum class GhostType {
    Blinky,  // Rojo
    Pinky,   // Rosa
    Inky,    // Cyan
    Clyde    // Naranja
};

enum class GhostMode {
    Scatter,     // Ir a esquina asignada
    Chase,       // Perseguir a Pac-Man
    Frightened,  // Huir (vulnerable)
    Eyes         // Volviendo a casa
};

class Ghost : public Entity {
public:
    Ghost(GhostType type);
    
    void update(float dt) override;
    void reset() override;
    
    // Getters
    GhostType getType() const { return type; }
    GhostMode getMode() const { return mode; }
    
    // Setters
    void setMode(GhostMode m);
    void setTarget(const Vector2& t) { target = t; }
    void setSpeedMultiplier(float mult) { speedMultiplier = mult; }
    
    // Control
    void reverseDirection();
    void sendToHouse();
    
    // Estado
    bool isInHouse() const { return inHouse; }
    
    // Animación
    int getAnimFrame() const { return animFrame; }
    bool isBlinking() const { return blinking; }
    void setBlinking(bool b) { blinking = b; }
    
    // Render
    void render(SDL_Renderer* renderer);
    
private:
    GhostType type;
    GhostMode mode = GhostMode::Scatter;
    float speedMultiplier = 1.0f;
    
    Vector2 target;
    
    // Casa de fantasmas
    bool inHouse = true;
    bool exitingHouse = false;
    bool enteringHouse = false;
    float houseTimer = 0.0f;
    int exitPhase = 0;  // 0=esperar, 1=centrar X, 2=subir
    
    // Animación
    int animFrame = 0;
    float animTimer = 0.0f;
    bool blinking = false;
    float blinkTimer = 0.0f;
    bool blinkState = false;
    
    // Movimiento
    bool canMove(Direction dir) const;
    Direction chooseDirection() const;
    void handleTunnelWrap();
    float getExitDelay() const;
    
    // Gráficos
    std::string getTextureKey() const;
};
