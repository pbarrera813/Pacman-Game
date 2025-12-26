// Game.h
// Clase principal del juego Pac-Man - Versión Final
#pragma once

#include "Pacman.h"
#include "Ghost.h"
#include "Renderer.h"
#include "AudioManager.h"
#include <vector>
#include <string>

// Estados del juego
enum class GameState {
    PressStart,   // Esperando Enter para comenzar
    Startup,      // Reproduciendo música de inicio
    Ready,        // Mostrando "READY!"
    Playing,      // Jugando
    Paused,       // Juego pausado
    GhostEaten,   // Congelado después de comer fantasma (1 segundo)
    PreDeath,     // Pausa antes de animación de muerte (1 segundo)
    Death,        // Animación de muerte
    GameOver,     // Game Over
    LevelClear    // Nivel completado (parpadeo del mapa)
};

// Tipos de fruta
enum class FruitType {
    Cherry,      // Nivel 1 - 100 pts
    Strawberry,  // Nivel 2 - 200 pts
    Orange,      // Nivel 3-4 - 500 pts
    Apple        // Nivel 5+ - 700 pts
};

// Información de fruta
struct FruitInfo {
    FruitType type;
    std::string textureKey;
    int points;
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
    
    // Freeze timers (para comer fantasma y pre-muerte)
    float freezeTimer = 0.0f;
    static constexpr float FREEZE_TIME = 0.5f;  // 0.5 segundos de congelación
    
    // Level Clear animation
    float levelClearTimer = 0.0f;
    float levelClearBlinkTimer = 0.0f;
    bool levelClearBlinkState = false;
    static constexpr float LEVEL_CLEAR_DURATION = 3.0f;
    static constexpr float LEVEL_CLEAR_BLINK_SPEED = 0.15f;
    
    // Puntuación
    int score = 0;
    int highScore = 0;
    int previousHighScore = 0;
    bool highScoreBeaten = false;
    float highScoreBlinkTimer = 0.0f;
    float highScoreBlinkAccum = 0.0f;  // Acumulador para parpadeo
    bool highScoreBlinkState = false;
    
    // High score reset
    float highScoreResetBlinkTimer = 0.0f;
    bool highScoreResetBlinkState = false;
    
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
    float fruitRespawnTimer = 0.0f;
    static constexpr float FRUIT_RESPAWN_TIME = 35.0f;  // 35 segundos para respawn
    std::vector<FruitType> collectedFruits;
    
    // Sirena
    bool sirenFast = false;
    
    // Entidades
    PacMan pacman;
    std::vector<Ghost> ghosts;
    
    // Sistemas
    Renderer renderer;
    
    // High score file
    static constexpr const char* HIGHSCORE_FILE = "highscore.dat";
    
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
    
    // Nuevos métodos
    FruitInfo getCurrentFruitInfo() const;
    float getSpeedMultiplier() const;
    void updateLevelClearAnimation(float dt);
    void renderLevelClearAnimation();
    void checkHighScore();
    void updateHighScoreBlink(float dt);
    void renderHUD();
    void renderFruitDisplay();
    
    // High score persistence
    void loadHighScore();
    void saveHighScore();
    void resetHighScore();
};
