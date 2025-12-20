// Game.h
// Clase principal del juego Pac-Man
#pragma once

#include "Pacman.h"
#include "Ghost.h"
#include "Renderer.h"
#include "AudioManager.h"
#include <vector>

// Estados del juego
enum class GameState {
    PressStart,  // Esperando Enter para comenzar
    Startup,     // Reproduciendo música de inicio
    Ready,       // Mostrando "READY!"
    Playing,     // Jugando
    Paused,      // Juego pausado
    Death,       // Animación de muerte
    GameOver,    // Game Over
    LevelClear   // Nivel completado
};

// Puntaje flotante
struct FloatingScore {
    int value;
    float x, y;
    float timer;
    bool active;
};

class Game {
public:
    Game();
    ~Game();
    
    bool init();
    void handleInput();
    void update(float dt);
    void render();
    
    bool isRunning() const { return running; }
    
private:
    GameState state = GameState::PressStart;
    GameState stateBeforePause = GameState::Playing;
    bool running = true;
    
    // Timers
    float stateTimer = 0.0f;
    float frightenedTimer = 0.0f;
    float scatterChaseTimer = 0.0f;
    float fruitTimer = 0.0f;
    float blinkTimer = 0.0f;
    bool blinkState = false;
    
    // Puntuación
    int score = 0;
    int highScore = 0;
    int lives = 3;
    int level = 1;
    int dotsEaten = 0;
    int ghostsEatenInFright = 0;
    
    // Puntajes flotantes
    std::vector<FloatingScore> floatingScores;
    static constexpr float FLOATING_SCORE_TIME = 1.0f;
    
    // Modo scatter/chase
    bool inScatterMode = true;
    int scatterChasePhase = 0;
    
    // Fruta
    bool fruitVisible = false;
    bool fruitEaten = false;
    
    // Sirena
    bool sirenFast = false;
    
    // Entidades
    PacMan pacman;
    std::vector<Ghost> ghosts;
    
    // Sistemas
    Renderer renderer;
    
    // Métodos
    void loadAllTextures();
    void startLevel();
    void resetPositions();
    void updatePlaying(float dt);
    void checkCollisions();
    void activateFrightenedMode();
    void updateScatterChaseMode(float dt);
    void updateSiren();
    void eatGhost(Ghost& ghost);
    void pacmanDied();
    void checkLevelComplete();
    void spawnFruit();
    void addFloatingScore(int value, float x, float y);
    void updateFloatingScores(float dt);
    void renderFloatingScores();
    void drawPausedText();
};
