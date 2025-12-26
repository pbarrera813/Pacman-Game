// Ghost.cpp
// Lógica de fantasmas
#include "Ghost.h"
#include "Map.h"
#include "Constants.h"
#include "TextureManager.h"
#include <cmath>

// Posiciones clave de la casa de fantasmas (en tiles)
// La puerta "--" está en fila 12, columnas 13-14
static constexpr int HOUSE_DOOR_X = 13;       // Columna de la puerta (izquierda)
static constexpr int HOUSE_DOOR_Y = 12;       // Fila de la puerta
static constexpr int HOUSE_ABOVE_DOOR_Y = 11; // Espacio arriba de la puerta
static constexpr int HOUSE_CENTER_Y = 14;     // Centro interior de la casa

Ghost::Ghost(GhostType t) : type(t) {
    reset();
}

void Ghost::reset() {
    mode = GhostMode::Scatter;
    speed = GHOST_SPEED;
    speedMultiplier = 1.0f;
    enteringHouse = false;
    exitPhase = 0;
    
    animFrame = 0;
    animTimer = 0.0f;
    blinking = false;
    blinkTimer = 0.0f;
    blinkState = false;
    houseTimer = 0.0f;
    
    // Posiciones iniciales según tipo
    switch (type) {
        case GhostType::Blinky:
            // Blinky empieza FUERA de la casa, arriba de la puerta
            position.x = (HOUSE_DOOR_X * TILE_SIZE + TILE_SIZE / 2) * SCALE;
            position.y = HOUSE_ABOVE_DOOR_Y * TILE_SIZE * SCALE;
            direction = Direction::Left;
            inHouse = false;
            exitingHouse = false;
            break;
            
        case GhostType::Pinky:
            // Pinky en el centro de la casa
            position.x = (HOUSE_DOOR_X * TILE_SIZE + TILE_SIZE / 2) * SCALE;
            position.y = HOUSE_CENTER_Y * TILE_SIZE * SCALE;
            direction = Direction::Down;
            inHouse = true;
            exitingHouse = false;
            break;
            
        case GhostType::Inky:
            // Inky a la izquierda en la casa
            position.x = (11 * TILE_SIZE + TILE_SIZE / 2) * SCALE;
            position.y = HOUSE_CENTER_Y * TILE_SIZE * SCALE;
            direction = Direction::Up;
            inHouse = true;
            exitingHouse = false;
            break;
            
        case GhostType::Clyde:
            // Clyde a la derecha en la casa
            position.x = (15 * TILE_SIZE + TILE_SIZE / 2) * SCALE;
            position.y = HOUSE_CENTER_Y * TILE_SIZE * SCALE;
            direction = Direction::Up;
            inHouse = true;
            exitingHouse = false;
            break;
    }
}

float Ghost::getExitDelay() const {
    switch (type) {
        case GhostType::Blinky: return 0.0f;
        case GhostType::Pinky:  return 0.0f;
        case GhostType::Inky:   return 3.0f;   // Reducido para que salga antes
        case GhostType::Clyde:  return 6.0f;   // Reducido
        default: return 0.0f;
    }
}

void Ghost::setMode(GhostMode m) {
    if (mode == m) return;
    
    GhostMode oldMode = mode;
    mode = m;
    
    if ((oldMode == GhostMode::Chase || oldMode == GhostMode::Scatter) 
        && m == GhostMode::Frightened) {
        reverseDirection();
        speed = GHOST_FRIGHT_SPEED;
    }
    else if (m == GhostMode::Eyes) {
        speed = GHOST_EYES_SPEED;
    }
    else if (m == GhostMode::Chase || m == GhostMode::Scatter) {
        speed = GHOST_SPEED;
    }
}

void Ghost::reverseDirection() {
    direction = oppositeDirection(direction);
}

void Ghost::sendToHouse() {
    mode = GhostMode::Eyes;
    speed = GHOST_EYES_SPEED;
    enteringHouse = false;
}

bool Ghost::canMove(Direction dir) const {
    int tx = getTileX();
    int ty = getTileY();
    
    switch (dir) {
        case Direction::Up:    ty--; break;
        case Direction::Down:  ty++; break;
        case Direction::Left:  tx--; break;
        case Direction::Right: tx++; break;
        default: return false;
    }
    
    // En modo Eyes puede atravesar la puerta
    bool canUseGhostDoor = (mode == GhostMode::Eyes);
    return Map::get().isWalkable(tx, ty, canUseGhostDoor);
}

Direction Ghost::chooseDirection() const {
    int tx = getTileX();
    int ty = getTileY();
    
    Direction best = Direction::None;
    float bestDist = 1e9f;
    
    // Prioridad del arcade: Up, Left, Down, Right
    Direction priorities[] = {Direction::Up, Direction::Left, Direction::Down, Direction::Right};
    
    for (Direction d : priorities) {
        // No puede dar vuelta en U
        if (d == oppositeDirection(direction))
            continue;
        
        if (!canMove(d))
            continue;
        
        int nx = tx, ny = ty;
        switch (d) {
            case Direction::Up:    ny--; break;
            case Direction::Down:  ny++; break;
            case Direction::Left:  nx--; break;
            case Direction::Right: nx++; break;
            default: break;
        }
        
        float dx = (nx * TILE_SIZE * SCALE) - target.x;
        float dy = (ny * TILE_SIZE * SCALE) - target.y;
        float dist = dx * dx + dy * dy;
        
        // En Frightened, maximizar distancia
        if (mode == GhostMode::Frightened) {
            dist = -dist;
        }
        
        if (dist < bestDist) {
            bestDist = dist;
            best = d;
        }
    }
    
    // Si no hay opción válida, intentar la opuesta
    if (best == Direction::None) {
        Direction opp = oppositeDirection(direction);
        if (canMove(opp)) {
            return opp;
        }
        return direction;
    }
    
    return best;
}

void Ghost::handleTunnelWrap() {
    float leftBound = -SCALED_TILE;
    float rightBound = MAP_WIDTH * SCALED_TILE;
    
    if (position.x < leftBound) {
        position.x = rightBound;
    }
    else if (position.x > rightBound) {
        position.x = leftBound;
    }
}

void Ghost::update(float dt) {
    // Animación siempre activa
    animTimer += dt;
    if (animTimer >= GHOST_ANIM_SPEED) {
        animTimer = 0.0f;
        animFrame = (animFrame + 1) % 2;
    }
    
    // Parpadeo en modo Frightened
    if (blinking && mode == GhostMode::Frightened) {
        blinkTimer += dt;
        if (blinkTimer >= BLINK_SPEED) {
            blinkTimer = 0.0f;
            blinkState = !blinkState;
        }
    }
    
    // ===== DENTRO DE LA CASA =====
    if (inHouse) {
        houseTimer += dt;
        
        // Esperar delay para empezar a salir
        if (!exitingHouse && houseTimer >= getExitDelay()) {
            exitingHouse = true;
            exitPhase = 0;
        }
        
        if (exitingHouse) {
            float exitSpeed = speed * dt * 0.8f;
            float centerX = (HOUSE_DOOR_X * TILE_SIZE + TILE_SIZE / 2) * SCALE;
            float exitY = HOUSE_ABOVE_DOOR_Y * TILE_SIZE * SCALE;
            
            // Fase 0: Moverse al centro horizontal
            if (exitPhase == 0) {
                float diffX = centerX - position.x;
                if (std::abs(diffX) > exitSpeed) {
                    position.x += (diffX > 0) ? exitSpeed : -exitSpeed;
                } else {
                    position.x = centerX;
                    exitPhase = 1;
                }
            }
            // Fase 1: Subir hacia la salida
            else if (exitPhase == 1) {
                position.y -= exitSpeed;
                if (position.y <= exitY) {
                    position.y = exitY;
                    // Salir exitoso
                    inHouse = false;
                    exitingHouse = false;
                    exitPhase = 0;
                    direction = Direction::Left;
                    houseTimer = 0.0f;
                }
            }
        }
        return;  // No hacer nada más si está en la casa
    }
    
    // ===== MODO EYES: Volviendo a la casa =====
    if (mode == GhostMode::Eyes) {
        // Posiciones clave en píxeles
        float doorCenterX = (HOUSE_DOOR_X * TILE_SIZE + TILE_SIZE / 2) * SCALE;
        float aboveDoorY = HOUSE_ABOVE_DOOR_Y * TILE_SIZE * SCALE;
        float insideY = HOUSE_CENTER_Y * TILE_SIZE * SCALE;
        
        // Verificar si esta en casa
        if (enteringHouse) {
            position.y += speed * dt * 0.8f;
            if (position.y >= insideY) {
                position.y = insideY;
                // Llegó al centro, regenerarse
                enteringHouse = false;
                inHouse = true;
                exitingHouse = true;
                exitPhase = 1;  // Ya está centrado, solo subir
                houseTimer = 0.0f;
                mode = GhostMode::Scatter;
                speed = GHOST_SPEED;
            }
            return;
        }
        
        // Verificar si llegó arriba de la puerta
        int tx = getTileX();
        int ty = getTileY();
        
        // Verificar si esta arriba de la puerta
        if ((tx == HOUSE_DOOR_X || tx == HOUSE_DOOR_X + 1) && ty == HOUSE_ABOVE_DOOR_Y) {
            // Centrar horizontalmente y empezar a bajar
            float diffX = doorCenterX - position.x;
            if (std::abs(diffX) > speed * dt) {
                position.x += (diffX > 0) ? speed * dt : -speed * dt;
            } else {
                position.x = doorCenterX;
                enteringHouse = true;
                direction = Direction::Down;
            }
            return;
        }
        
        // Target: el espacio arriba de la puerta
        target.x = doorCenterX;
        target.y = aboveDoorY;
    }
    
    // MOVIMIENTO NORMAL
    // Solo cambiar dirección cuando está centrado en un tile
    if (isCentered()) {
        direction = chooseDirection();
    }
    
    // Velocidad (más lento en túneles excepto en modo Eyes)
    float currentSpeed = speed * speedMultiplier;  // Aplicar multiplicador de nivel
    if (mode != GhostMode::Eyes && Map::get().isTunnel(getTileX(), getTileY())) {
        currentSpeed = GHOST_TUNNEL_SPEED * speedMultiplier;
    }
    
    // Mover
    switch (direction) {
        case Direction::Up:    position.y -= currentSpeed * dt; break;
        case Direction::Down:  position.y += currentSpeed * dt; break;
        case Direction::Left:  position.x -= currentSpeed * dt; break;
        case Direction::Right: position.x += currentSpeed * dt; break;
        default: break;
    }
    
    handleTunnelWrap();
}

std::string Ghost::getTextureKey() const {
    if (mode == GhostMode::Eyes) {
        switch (direction) {
            case Direction::Up:    return "ghost_eyes_up";
            case Direction::Down:  return "ghost_eyes_down";
            case Direction::Left:  return "ghost_eyes_left";
            case Direction::Right: return "ghost_eyes_right";
            default:               return "ghost_eyes_left";
        }
    }
    
    if (mode == GhostMode::Frightened) {
        if (blinking && blinkState) {
            return (animFrame == 0) ? "ghost_afraid_1" : "ghost_afraid_0";
        }
        return (animFrame == 0) ? "ghost_afraid_0" : "ghost_afraid_1";
    }
    
    std::string color;
    switch (type) {
        case GhostType::Blinky: color = "red";    break;
        case GhostType::Pinky:  color = "pink";   break;
        case GhostType::Inky:   color = "blue";   break;
        case GhostType::Clyde:  color = "orange"; break;
    }
    
    return "ghost_" + color + "_" + std::to_string(animFrame);
}

void Ghost::render(SDL_Renderer* /*renderer*/) {
    TextureManager::get().draw(
        getTextureKey(),
        static_cast<int>(position.x),
        static_cast<int>(position.y) + GAME_OFFSET_Y,
        SCALED_TILE,
        SCALED_TILE
    );
}