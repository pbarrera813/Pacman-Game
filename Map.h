// Map.h
// Mapa del laberinto de Pac-Man
#pragma once

#include "Constants.h"

enum class TileType {
    Empty,
    Wall,
    Dot,
    PowerPellet,
    GhostDoor,
    Tunnel
};

class Map {
public:
    // Singleton
    static Map& get();
    
    // Acceso a tiles
    TileType getTile(int x, int y) const;
    void setTile(int x, int y, TileType type);
    
    // Movimiento
    bool isWalkable(int x, int y, bool isGhost = false) const;
    bool isTunnel(int x, int y) const;
    bool isIntersection(int x, int y) const;
    
    // Dots
    bool eatDot(int x, int y);
    bool eatPowerPellet(int x, int y);
    int getRemainingDots() const;
    int getTotalDots() const { return totalDots; }
    
    // Reset
    void resetLevel();
    
private:
    Map();
    void loadMap();
    
    TileType tiles[MAP_HEIGHT][MAP_WIDTH];
    int totalDots = 0;
    int remainingDots = 0;
};
