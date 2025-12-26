// Constants.h
// Constantes fieles al Pac-Man arcade original
#pragma once

// Mapa (dimensiones del arcade original)
constexpr int MAP_WIDTH  = 28;
constexpr int MAP_HEIGHT = 31;
constexpr int TILE_SIZE  = 8;   // Tamaño de tile del arcade

// Escala de pantalla (para hacer el juego visible)
constexpr int SCALE = 3;
constexpr int SCALED_TILE = TILE_SIZE * SCALE;

// Pantalla
constexpr int SCREEN_WIDTH  = MAP_WIDTH  * TILE_SIZE * SCALE;  // 672
constexpr int SCREEN_HEIGHT = (MAP_HEIGHT * TILE_SIZE + 16) * SCALE; // HUD arriba y abajo

// Offset para el área de juego (HUD arriba)
constexpr int GAME_OFFSET_Y = 24 * SCALE;  // Espacio para score arriba

// Velocidades (píxeles por segundo, escaladas)
// El Pac-Man original corre a ~80% de velocidad base
constexpr float BASE_SPEED = 75.75f * SCALE;  // Velocidad base
constexpr float PACMAN_SPEED = BASE_SPEED * 0.80f;
constexpr float PACMAN_FRIGHT_SPEED = BASE_SPEED * 0.90f;
constexpr float PACMAN_DOT_SPEED = BASE_SPEED * 0.71f;

constexpr float GHOST_SPEED = BASE_SPEED * 0.75f;
constexpr float GHOST_FRIGHT_SPEED = BASE_SPEED * 0.50f;
constexpr float GHOST_TUNNEL_SPEED = BASE_SPEED * 0.40f;
constexpr float GHOST_EYES_SPEED = BASE_SPEED * 1.50f;

// Puntuaciones (arcade original)
constexpr int SCORE_DOT = 10;
constexpr int SCORE_POWER_PELLET = 50;
constexpr int SCORE_CHERRY = 100;
constexpr int SCORE_GHOST_1 = 200;   // Primer fantasma
constexpr int SCORE_GHOST_2 = 400;   // Segundo fantasma
constexpr int SCORE_GHOST_3 = 800;   // Tercer fantasma
constexpr int SCORE_GHOST_4 = 1600;  // Cuarto fantasma

// Timers (segundos)
constexpr float STARTUP_TIME = 4.2f;       // Duración del sonido de inicio
constexpr float READY_TIME = 2.0f;         // Tiempo mostrando "READY!"
constexpr float FRIGHTENED_TIME = 6.0f;    // Duración del modo asustado (nivel 1)
constexpr float FRIGHTENED_BLINK_TIME = 2.0f; // Últimos segundos parpadea
constexpr float DEATH_ANIM_TIME = 1.5f;    // Duración animación de muerte
constexpr float LEVEL_CLEAR_TIME = 2.0f;   // Tiempo antes de siguiente nivel
constexpr float FRUIT_VISIBLE_TIME = 10.0f; // Tiempo que la fruta está visible

// Posiciones iniciales (en tiles)
// La posición debe estar en un tile caminable
constexpr int PACMAN_START_X = 14;  // Centro del mapa (fila 22: "...##................##...")
constexpr int PACMAN_START_Y = 22;  // Corredor horizontal con pellets

constexpr int GHOST_HOUSE_X = 13;
constexpr int GHOST_HOUSE_Y = 14;

// Scatter corners (esquinas de dispersión)
constexpr int BLINKY_SCATTER_X = 25;
constexpr int BLINKY_SCATTER_Y = 0;

constexpr int PINKY_SCATTER_X = 2;
constexpr int PINKY_SCATTER_Y = 0;

constexpr int INKY_SCATTER_X = 27;
constexpr int INKY_SCATTER_Y = 30;

constexpr int CLYDE_SCATTER_X = 0;
constexpr int CLYDE_SCATTER_Y = 30;

// Animación
constexpr float PACMAN_ANIM_SPEED = 0.05f;
constexpr float GHOST_ANIM_SPEED = 0.15f;
constexpr float BLINK_SPEED = 0.15f;
