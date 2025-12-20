// Map.cpp
// Laberinto fiel al Pac-Man arcade original
#include "Map.h"

Map& Map::get() {
    static Map instance;
    return instance;
}

Map::Map() {
    loadMap();
}

void Map::loadMap() {
    // Mapa del Pac-Man arcade original
    // # = Pared
    // . = Dot
    // o = Power Pellet
    // - = Puerta de la casa de fantasmas
    // T = Túnel
    // (espacio) = Vacío
    
    const char* rawMap[MAP_HEIGHT] = {
        "############################",  // 0
        "#............##............#",  // 1
        "#.####.#####.##.#####.####.#",  // 2
        "#o####.#####.##.#####.####o#",  // 3
        "#.####.#####.##.#####.####.#",  // 4
        "#..........................#",  // 5
        "#.####.##.########.##.####.#",  // 6
        "#.####.##.########.##.####.#",  // 7
        "#......##....##....##......#",  // 8
        "######.##### ## #####.######",  // 9
        "     #.##### ## #####.#     ",  // 10
        "     #.##          ##.#     ",  // 11
        "     #.## ###--### ##.#     ",  // 12
        "######.## #      # ##.######",  // 13
        "TTTTTT.   #      #   .TTTTTT",  // 14 - Túnel
        "######.## #      # ##.######",  // 15
        "     #.## ######## ##.#     ",  // 16
        "     #.##          ##.#     ",  // 17
        "     #.## ######## ##.#     ",  // 18
        "######.## ######## ##.######",  // 19
        "#............##............#",  // 20
        "#.####.#####.##.#####.####.#",  // 21
        "#o..##................##..o#",  // 22
        "###.##.##.########.##.##.###",  // 23
        "###.##.##.########.##.##.###",  // 24
        "#......##....##....##......#",  // 25
        "#.##########.##.##########.#",  // 26
        "#.##########.##.##########.#",  // 27
        "#..........................#",  // 28
        "############################",  // 29
        "############################"   // 30
    };
    
    totalDots = 0;
    remainingDots = 0;
    
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            char c = rawMap[y][x];
            switch (c) {
                case '#': 
                    tiles[y][x] = TileType::Wall; 
                    break;
                case '.': 
                    tiles[y][x] = TileType::Dot;
                    totalDots++;
                    remainingDots++;
                    break;
                case 'o': 
                    tiles[y][x] = TileType::PowerPellet;
                    totalDots++;
                    remainingDots++;
                    break;
                case '-': 
                    tiles[y][x] = TileType::GhostDoor; 
                    break;
                case 'T':
                    tiles[y][x] = TileType::Tunnel;
                    break;
                case ' ':
                default:
                    tiles[y][x] = TileType::Empty;
                    break;
            }
        }
    }
}

TileType Map::getTile(int x, int y) const {
    // Wrap horizontal para túneles
    if (x < 0) x += MAP_WIDTH;
    if (x >= MAP_WIDTH) x -= MAP_WIDTH;
    
    if (y < 0 || y >= MAP_HEIGHT)
        return TileType::Wall;
        
    return tiles[y][x];
}

void Map::setTile(int x, int y, TileType type) {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
        return;
    tiles[y][x] = type;
}

bool Map::isWalkable(int x, int y, bool isGhost) const {
    TileType t = getTile(x, y);
    
    if (t == TileType::Wall)
        return false;
    
    if (t == TileType::GhostDoor)
        return isGhost;
    
    return true;
}

bool Map::isTunnel(int x, int y) const {
    return getTile(x, y) == TileType::Tunnel;
}

bool Map::isIntersection(int x, int y) const {
    // Cuenta cuántas direcciones son transitables
    int count = 0;
    if (isWalkable(x, y - 1, false)) count++;
    if (isWalkable(x, y + 1, false)) count++;
    if (isWalkable(x - 1, y, false)) count++;
    if (isWalkable(x + 1, y, false)) count++;
    return count > 2;
}

bool Map::eatDot(int x, int y) {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
        return false;
    
    if (tiles[y][x] == TileType::Dot) {
        tiles[y][x] = TileType::Empty;
        remainingDots--;
        return true;
    }
    return false;
}

bool Map::eatPowerPellet(int x, int y) {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
        return false;
    
    if (tiles[y][x] == TileType::PowerPellet) {
        tiles[y][x] = TileType::Empty;
        remainingDots--;
        return true;
    }
    return false;
}

int Map::getRemainingDots() const {
    return remainingDots;
}

void Map::resetLevel() {
    loadMap();
}