// Game.h
// Clase principal del juego Pac-Man - Versión 3.2.1
#pragma once

#include "Pacman.h"
#include "Ghost.h"
#include "Renderer.h"
#include "AudioManager.h"
#include <SDL2/SDL.h>
#include <vector>
#include <string>

// Estados del juego
enum class GameState {
    PressStart,   // Esperando Enter para comenzar
    Startup,      // Reproduciendo música de inicio
    Ready,        // Mostrando "READY!"
    Playing,      // Jugando
    Paused,       // Juego pausado
    GhostEaten,   // Congelado después de comer fantasma (0.5 segundos)
    PreDeath,     // Pausa antes de animación de muerte (0.5 segundos)
    Death,        // Animación de muerte
    GameOver,     // Game Over
    LevelClear    // Nivel completado (parpadeo del mapa)
};

// Tipos de fruta (8 frutas total)
enum class FruitType {
    Cherry,      // Nivel 1 - 100 pts
    Strawberry,  // Nivel 2 - 300 pts
    Orange,      // Nivel 3-4 - 500 pts
    Apple,       // Nivel 5-6 - 700 pts
    Melon,       // Nivel 7-8 - 1000 pts
    Ship,        // Nivel 9-10 - 2000 pts (Galaxian)
    Bell,        // Nivel 11-12 - 3000 pts
    Key          // Nivel 13+ - 5000 pts
};

// Información de fruta
struct FruitInfo {
    FruitType type;
    std::string textureKey;
    std::string pointsTextureKey;  // Sprite del puntaje
    int points;
};

// Puntaje flotante (ahora usa sprites)
struct FloatingScore {
    std::string textureKey;  // Sprite del puntaje a mostrar
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
    static constexpr float FREEZE_TIME = 0.5f;
    
    // Timer para sonido waka (se detiene si no come en X tiempo)
    float wakaTimer = 0.0f;
    static constexpr float WAKA_TIMEOUT = 0.25f;  // Tiempo antes de detener waka
    bool wakaPlaying = false;
    
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
    float highScoreBlinkAccum = 0.0f;
    bool highScoreBlinkState = false;
    
    // High score reset
    float highScoreResetBlinkTimer = 0.0f;
    bool highScoreResetBlinkState = false;
    
    int lives = 3;
    int level = 1;
    int dotsEaten = 0;
    int ghostsEatenInFright = 0;
    
    // Puntajes flotantes (ahora con sprites)
    std::vector<FloatingScore> floatingScores;
    static constexpr float FLOATING_SCORE_TIME = 1.0f;
    
    // Modo scatter/chase
    bool inScatterMode = true;
    int scatterChasePhase = 0;
    
    // Fruta
    bool fruitVisible = false;
    bool fruitEaten = false;
    float fruitRespawnTimer = 0.0f;
    static constexpr float FRUIT_RESPAWN_TIME = 35.0f;
    std::vector<FruitType> collectedFruits;
    
    // Sirena
    bool sirenFast = false;
    
    // Control de volumen
    int volumeLevel = 100;  // 100, 50, 25, 0
    SDL_Rect volumeIconRect;  // Área clickeable del icono
    
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
    void addFloatingScore(const std::string& textureKey, float x, float y);
    void updateFloatingScores(float dt);
    void renderFloatingScores();
    void drawPausedText();
    
    // Métodos de frutas y niveles
    FruitInfo getCurrentFruitInfo() const;
    float getSpeedMultiplier() const;
    void updateLevelClearAnimation(float dt);
    void renderLevelClearAnimation();
    void checkHighScore();
    void updateHighScoreBlink(float dt);
    void renderHUD();
    void renderFruitDisplay();
    
    // High score persistence
    std::string getHighScorePath() const;
    void loadHighScore();
    void saveHighScore();
    void resetHighScore();
    
    // Helpers
    std::string getGhostPointsTexture(int ghostIndex) const;
    std::string getFruitTextureForLevel(int lvl) const;
    
    // Control de volumen
    void handleVolumeClick(int mouseX, int mouseY);
    void cycleVolume();
    void renderVolumeIcon();
    std::string getVolumeTextureKey() const;
};
