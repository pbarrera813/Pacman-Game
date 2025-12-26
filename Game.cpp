// Game.cpp - Pac-Man Versión Final
#include "Game.h"
#include "GhostAI.h"
#include "Map.h"
#include "TextureManager.h"
#include "Constants.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <fstream>

// Función para dibujar dígitos simples
static void drawDigitSimple(SDL_Renderer* renderer, char digit, int x, int y, int scale);

// Tiempos de Scatter/Chase
static const float SCATTER_TIMES[] = {7.0f, 7.0f, 5.0f, 5.0f};
static const float CHASE_TIMES[] = {20.0f, 20.0f, 20.0f, 999999.0f};

Game::Game() {}

Game::~Game() {
    saveHighScore();
    AudioManager::get().shutdown();
}

bool Game::init() {
    if (!renderer.init()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }
    
    TextureManager::get().init(renderer.getSDLRenderer());
    loadAllTextures();
    AudioManager::get().init();
    
    // Cargar high score guardado
    loadHighScore();
    previousHighScore = highScore;
    
    // Crear fantasmas
    ghosts.push_back(Ghost(GhostType::Blinky));
    ghosts.push_back(Ghost(GhostType::Pinky));
    ghosts.push_back(Ghost(GhostType::Inky));
    ghosts.push_back(Ghost(GhostType::Clyde));
    
    // Empezar en pantalla de título
    state = GameState::PressStart;
    
    return true;
}

void Game::loadHighScore() {
    std::ifstream file(HIGHSCORE_FILE, std::ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(&highScore), sizeof(highScore));
        file.close();
    } else {
        highScore = 0;
    }
}

void Game::saveHighScore() {
    std::ofstream file(HIGHSCORE_FILE, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&highScore), sizeof(highScore));
        file.close();
    }
}

void Game::resetHighScore() {
    highScore = 0;
    previousHighScore = 0;
    highScoreBeaten = false;
    saveHighScore();
    
    // Activar parpadeo de confirmación
    highScoreResetBlinkTimer = 1.5f;
    AudioManager::get().playSound(SoundID::Success);
}

void Game::loadAllTextures() {
    auto& tm = TextureManager::get();
    
    // Pac-Man
    tm.load("pacman_0", "assets/gfx/pacman_counter_death/pacman/pac_man_0.png");
    tm.load("pacman_1", "assets/gfx/pacman_counter_death/pacman/pac_man_1.png");
    tm.load("pacman_2", "assets/gfx/pacman_counter_death/pacman/pac_man_2.png");
    tm.load("pacman_3", "assets/gfx/pacman_counter_death/pacman/pac_man_3.png");
    tm.load("pacman_4", "assets/gfx/pacman_counter_death/pacman/pac_man_4.png");
    
    tm.load("pacman_death_0", "assets/gfx/pacman_counter_death/pacman_death/pacdeath_0.png");
    tm.load("pacman_death_1", "assets/gfx/pacman_counter_death/pacman_death/pacdeath_1.png");
    tm.load("pacman_death_2", "assets/gfx/pacman_counter_death/pacman_death/pacdeath_2.png");
    
    tm.load("pacman_life", "assets/gfx/pacman_counter_death/pacman_counter/lifecounter_0.png");
    
    // Fantasmas
    tm.load("ghost_red_0", "assets/gfx/ghost/red_ghost/ghost_red_0.png");
    tm.load("ghost_red_1", "assets/gfx/ghost/red_ghost/ghost_red_1.png");
    tm.load("ghost_pink_0", "assets/gfx/ghost/pink_ghost/ghost_pink_0.png");
    tm.load("ghost_pink_1", "assets/gfx/ghost/pink_ghost/ghost_pink_1.png");
    tm.load("ghost_blue_0", "assets/gfx/ghost/blue_ghost/ghost_blue_0.png");
    tm.load("ghost_blue_1", "assets/gfx/ghost/blue_ghost/ghost_blue_1.png");
    tm.load("ghost_orange_0", "assets/gfx/ghost/orange_ghost/ghost_orange_0.png");
    tm.load("ghost_orange_1", "assets/gfx/ghost/orange_ghost/ghost_orange_1.png");
    
    tm.load("ghost_afraid_0", "assets/gfx/ghost/ghost_afraid/afraid_0.png");
    tm.load("ghost_afraid_1", "assets/gfx/ghost/ghost_afraid/afraid_1.png");
    
    tm.load("ghost_eyes_up", "assets/gfx/ghost/eyes/eyes_up.png");
    tm.load("ghost_eyes_down", "assets/gfx/ghost/eyes/eyes_down.png");
    tm.load("ghost_eyes_left", "assets/gfx/ghost/eyes/eyes_left.png");
    tm.load("ghost_eyes_right", "assets/gfx/ghost/eyes/eyes_right.png");
    
    // Pills
    tm.load("pill", "assets/gfx/pill/pill_0.png");
    tm.load("super_pill", "assets/gfx/pill/pill_1.png");
    
    // Frutas
    tm.load("fruit_cherry", "assets/gfx/cherry/spr_cherry_0.png");
    tm.load("fruit_strawberry", "assets/gfx/cherry/spr_strawberry_0.png");
    tm.load("fruit_orange", "assets/gfx/cherry/spr_orange_0.png");
    tm.load("fruit_apple", "assets/gfx/cherry/spr_apple_0.png");
    
    // Textos
    tm.load("text_ready", "assets/gfx/pacman_text/ready.png");
    tm.load("text_gameover", "assets/gfx/pacman_text/game_over.png");
    tm.load("text_clear", "assets/gfx/pacman_text/clear.png");
    tm.load("text_pressstart", "assets/gfx/pacman_text/enter_0.png");
    tm.load("text_pause", "assets/gfx/pacman_text/pause.png");
}

FruitInfo Game::getCurrentFruitInfo() const {
    FruitInfo info;
    
    switch (level) {
        case 1:
            info.type = FruitType::Cherry;
            info.textureKey = "fruit_cherry";
            info.points = 100;
            break;
        case 2:
            info.type = FruitType::Strawberry;
            info.textureKey = "fruit_strawberry";
            info.points = 200;
            break;
        case 3:
        case 4:
            info.type = FruitType::Orange;
            info.textureKey = "fruit_orange";
            info.points = 500;
            break;
        default:  // Nivel 5+
            info.type = FruitType::Apple;
            info.textureKey = "fruit_apple";
            info.points = 700;
            break;
    }
    
    return info;
}

float Game::getSpeedMultiplier() const {
    float multiplier = 1.0f + (std::min(level - 1, 6) * 0.05f);
    return multiplier;
}

void Game::handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                case SDLK_w:
                    if (state == GameState::Playing)
                        pacman.setDesiredDirection(Direction::Up);
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                    if (state == GameState::Playing)
                        pacman.setDesiredDirection(Direction::Down);
                    break;
                case SDLK_LEFT:
                case SDLK_a:
                    if (state == GameState::Playing)
                        pacman.setDesiredDirection(Direction::Left);
                    break;
                case SDLK_RIGHT:
                case SDLK_d:
                    if (state == GameState::Playing)
                        pacman.setDesiredDirection(Direction::Right);
                    break;
                    
                case SDLK_r:
                    // Reset high score (en PressStart, GameOver o Paused)
                    if (state == GameState::PressStart || 
                        state == GameState::GameOver ||
                        state == GameState::Paused) {
                        resetHighScore();
                    }
                    break;
                    
                case SDLK_RETURN:
                    if (state == GameState::PressStart) {
                        state = GameState::Startup;
                        stateTimer = 0.0f;
                        AudioManager::get().playSound(SoundID::Startup);
                    }
                    else if (state == GameState::GameOver) {
                        // Guardar el récord actual como el récord a batir
                        previousHighScore = highScore;
                        // Reiniciar flags de high score para la nueva partida
                        highScoreBeaten = false;
                        highScoreBlinkTimer = 0.0f;
                        highScoreBlinkAccum = 0.0f;
                        highScoreBlinkState = false;
                        // Reiniciar partida
                        score = 0;
                        lives = 3;
                        level = 1;
                        collectedFruits.clear();
                        Map::get().resetLevel();
                        startLevel();
                    }
                    break;
                    
                case SDLK_ESCAPE:
                    if (state == GameState::Playing) {
                        stateBeforePause = state;
                        state = GameState::Paused;
                        // Primero detener sonidos de gameplay
                        AudioManager::get().stopSiren();
                        AudioManager::get().stopSound(SoundID::Waka);
                        AudioManager::get().stopSound(SoundID::BackToBase);
                        // Luego reproducir sonido de pausa
                        AudioManager::get().playSound(SoundID::Pause);
                    }
                    else if (state == GameState::Paused) {
                        state = stateBeforePause;
                        // Reproducir sonido de despausar
                        AudioManager::get().playSound(SoundID::Unpause);
                        // Reanudar sirena si corresponde
                        if (frightenedTimer <= 0.0f) {
                            AudioManager::get().playSiren(sirenFast);
                        }
                    }
                    else if (state == GameState::PressStart) {
                        running = false;
                    }
                    break;
            }
        }
    }
}

void Game::startLevel() {
    resetPositions();
    dotsEaten = 0;
    fruitVisible = false;
    fruitEaten = false;
    fruitRespawnTimer = 0.0f;
    frightenedTimer = 0.0f;
    ghostsEatenInFright = 0;
    floatingScores.clear();
    
    inScatterMode = true;
    scatterChasePhase = 0;
    scatterChaseTimer = SCATTER_TIMES[0];
    
    state = GameState::Ready;
    stateTimer = READY_TIME;
}

void Game::resetPositions() {
    pacman.reset();
    pacman.setSpeedMultiplier(getSpeedMultiplier());
    
    for (auto& ghost : ghosts) {
        ghost.reset();
        ghost.setSpeedMultiplier(getSpeedMultiplier());
    }
}

void Game::update(float dt) {
    // Parpadeo para textos
    blinkTimer += dt;
    if (blinkTimer >= 0.3f) {
        blinkTimer = 0.0f;
        blinkState = !blinkState;
    }
    
    // Actualizar parpadeos
    updateHighScoreBlink(dt);
    
    // Parpadeo de reset de high score
    if (highScoreResetBlinkTimer > 0.0f) {
        highScoreResetBlinkTimer -= dt;
        static float resetBlinkAccum = 0.0f;
        resetBlinkAccum += dt;
        if (resetBlinkAccum >= 0.1f) {
            resetBlinkAccum = 0.0f;
            highScoreResetBlinkState = !highScoreResetBlinkState;
        }
        if (highScoreResetBlinkTimer <= 0.0f) {
            highScoreResetBlinkState = false;
        }
    }
    
    switch (state) {
        case GameState::PressStart:
            break;
            
        case GameState::Startup:
            stateTimer += dt;
            if (!AudioManager::get().isPlaying(SoundID::Startup) && stateTimer > 0.5f) {
                startLevel();
            }
            break;
            
        case GameState::Ready:
            stateTimer -= dt;
            if (stateTimer <= 0.0f) {
                state = GameState::Playing;
                AudioManager::get().playSiren(false);
            }
            break;
            
        case GameState::Playing:
            updatePlaying(dt);
            break;
            
        case GameState::Paused:
            break;
            
        case GameState::GhostEaten:
            // Congelado después de comer fantasma
            freezeTimer -= dt;
            if (freezeTimer <= 0.0f) {
                state = GameState::Playing;
                // b2b.wav ya está sonando desde eatGhost()
                // Reanudar sirena si no hay modo frightened activo
                if (frightenedTimer <= 0.0f) {
                    AudioManager::get().playSiren(sirenFast);
                }
            }
            // Actualizar solo puntajes flotantes durante el freeze
            updateFloatingScores(dt);
            break;
            
        case GameState::PreDeath:
            // Pausa antes de animación de muerte
            freezeTimer -= dt;
            if (freezeTimer <= 0.0f) {
                state = GameState::Death;
                AudioManager::get().playSound(SoundID::Death);
                pacman.die();
            }
            break;
            
        case GameState::Death:
            pacman.updateDeathAnimation(dt);
            if (pacman.isDeathAnimationComplete()) {
                lives--;
                if (lives <= 0) {
                    state = GameState::GameOver;
                    saveHighScore();
                }
                else {
                    resetPositions();
                    state = GameState::Ready;
                    stateTimer = READY_TIME;
                }
            }
            break;
            
        case GameState::LevelClear:
            updateLevelClearAnimation(dt);
            break;
            
        case GameState::GameOver:
            break;
    }
}

void Game::updateLevelClearAnimation(float dt) {
    levelClearTimer += dt;
    levelClearBlinkTimer += dt;
    
    if (levelClearBlinkTimer >= LEVEL_CLEAR_BLINK_SPEED) {
        levelClearBlinkTimer = 0.0f;
        levelClearBlinkState = !levelClearBlinkState;
    }
    
    if (levelClearTimer >= LEVEL_CLEAR_DURATION) {
        level++;
        
        FruitInfo fruitInfo = getCurrentFruitInfo();
        
        if (collectedFruits.size() >= 5) {
            collectedFruits.erase(collectedFruits.begin());
        }
        collectedFruits.push_back(fruitInfo.type);
        
        Map::get().resetLevel();
        startLevel();
    }
}

void Game::updatePlaying(float dt) {
    updateScatterChaseMode(dt);
    
    pacman.update(dt);
    
    // Reproducir waka SOLO cuando come un pellet
    if (pacman.ateDot || pacman.atePowerPellet) {
        AudioManager::get().playSound(SoundID::Waka);
    }
    // NO detener waka aquí - dejar que termine naturalmente
    
    // Puntuación por dot
    if (pacman.ateDot) {
        score += SCORE_DOT;
        dotsEaten++;
        checkHighScore();
        spawnFruit();
    }
    
    // Power pellet
    if (pacman.atePowerPellet) {
        score += SCORE_POWER_PELLET;
        dotsEaten++;
        checkHighScore();
        activateFrightenedMode();
        spawnFruit();
    }
    
    // Puntajes flotantes
    updateFloatingScores(dt);
    
    // Timer frightened
    if (frightenedTimer > 0.0f) {
        frightenedTimer -= dt;
        
        bool shouldBlink = frightenedTimer <= FRIGHTENED_BLINK_TIME;
        for (auto& ghost : ghosts) {
            if (ghost.getMode() == GhostMode::Frightened) {
                ghost.setBlinking(shouldBlink);
            }
        }
        
        if (frightenedTimer <= 0.0f) {
            for (auto& ghost : ghosts) {
                if (ghost.getMode() == GhostMode::Frightened) {
                    ghost.setMode(inScatterMode ? GhostMode::Scatter : GhostMode::Chase);
                    ghost.setBlinking(false);
                }
            }
            ghostsEatenInFright = 0;
            AudioManager::get().playSiren(sirenFast);
        }
    }
    
    // Verificar si hay fantasmas en modo Eyes
    bool anyEyes = false;
    for (auto& ghost : ghosts) {
        if (ghost.getMode() == GhostMode::Eyes) {
            anyEyes = true;
            break;
        }
    }
    
    // Mantener b2b.wav sonando mientras haya ojos
    if (anyEyes) {
        if (!AudioManager::get().isPlaying(SoundID::BackToBase)) {
            AudioManager::get().playSound(SoundID::BackToBase, -1);
        }
    } else {
        AudioManager::get().stopSound(SoundID::BackToBase);
    }
    
    // Actualizar fantasmas
    Ghost* blinky = &ghosts[0];
    for (auto& ghost : ghosts) {
        Vector2 target = GhostAI::getTarget(ghost, pacman, blinky);
        ghost.setTarget(target);
        ghost.update(dt);
    }
    
    checkCollisions();
    updateSiren();
    checkLevelComplete();
    
    // Timer de fruta visible
    if (fruitVisible) {
        fruitTimer -= dt;
        if (fruitTimer <= 0.0f) {
            fruitVisible = false;
        }
    }
    
    // Timer de respawn de fruta (35 segundos después de ser comida)
    if (fruitEaten && !fruitVisible) {
        fruitRespawnTimer += dt;
        if (fruitRespawnTimer >= FRUIT_RESPAWN_TIME) {
            fruitVisible = true;
            fruitTimer = FRUIT_VISIBLE_TIME;
            fruitRespawnTimer = 0.0f;
            fruitEaten = false;  // Reset para permitir otro respawn
        }
    }
}

void Game::checkHighScore() {
    if (previousHighScore > 0 && !highScoreBeaten && score > previousHighScore) {
        highScoreBeaten = true;
        highScoreBlinkTimer = 2.0f;
        AudioManager::get().playSound(SoundID::HighScore);
    }
    
    if (score > highScore) {
        highScore = score;
    }
}

void Game::updateHighScoreBlink(float dt) {
    if (highScoreBlinkTimer > 0.0f) {
        highScoreBlinkTimer -= dt;
        
        // Usar acumulador miembro, no variable static
        highScoreBlinkAccum += dt;
        if (highScoreBlinkAccum >= 0.1f) {
            highScoreBlinkAccum = 0.0f;
            highScoreBlinkState = !highScoreBlinkState;
        }
        
        if (highScoreBlinkTimer <= 0.0f) {
            highScoreBlinkState = false;
            highScoreBlinkAccum = 0.0f;
        }
    }
}

void Game::updateScatterChaseMode(float dt) {
    if (frightenedTimer > 0.0f)
        return;
    
    scatterChaseTimer -= dt;
    
    if (scatterChaseTimer <= 0.0f) {
        inScatterMode = !inScatterMode;
        
        if (!inScatterMode) {
            scatterChaseTimer = CHASE_TIMES[scatterChasePhase];
        }
        else {
            scatterChasePhase++;
            if (scatterChasePhase >= 4) scatterChasePhase = 3;
            scatterChaseTimer = SCATTER_TIMES[scatterChasePhase];
        }
        
        GhostMode newMode = inScatterMode ? GhostMode::Scatter : GhostMode::Chase;
        for (auto& ghost : ghosts) {
            if (ghost.getMode() != GhostMode::Frightened && 
                ghost.getMode() != GhostMode::Eyes) {
                ghost.setMode(newMode);
            }
        }
    }
}

void Game::activateFrightenedMode() {
    frightenedTimer = FRIGHTENED_TIME;
    ghostsEatenInFright = 0;
    
    for (auto& ghost : ghosts) {
        if (ghost.getMode() != GhostMode::Eyes) {
            ghost.setMode(GhostMode::Frightened);
        }
    }
    
    AudioManager::get().stopSiren();
    AudioManager::get().playSound(SoundID::PowerUp);
}

void Game::checkCollisions() {
    int pacTileX = pacman.getTileX();
    int pacTileY = pacman.getTileY();
    
    // Colisión con fantasmas
    for (auto& ghost : ghosts) {
        int ghostTileX = ghost.getTileX();
        int ghostTileY = ghost.getTileY();
        
        if (pacTileX == ghostTileX && pacTileY == ghostTileY) {
            if (ghost.getMode() == GhostMode::Frightened) {
                eatGhost(ghost);
                return;  // Importante: salir para evitar múltiples colisiones
            }
            else if (ghost.getMode() != GhostMode::Eyes) {
                pacmanDied();
                return;
            }
        }
    }
    
    // Colisión con fruta - usar detección más precisa
    if (fruitVisible) {
        // La fruta está en el centro del mapa, tile (13, 17)
        // Verificar si Pac-Man está cerca de la fruta
        float fruitX = 13.5f * SCALED_TILE;  // Centro entre tiles 13-14
        float fruitY = 17.0f * SCALED_TILE;
        
        float pacCenterX = pacman.position.x + SCALED_TILE / 2.0f;
        float pacCenterY = pacman.position.y + SCALED_TILE / 2.0f;
        
        float dx = pacCenterX - fruitX;
        float dy = pacCenterY - fruitY;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        // Si está dentro de medio tile, comer la fruta
        if (distance < SCALED_TILE * 0.6f) {
            FruitInfo fruitInfo = getCurrentFruitInfo();
            score += fruitInfo.points;
            checkHighScore();
            addFloatingScore(fruitInfo.points, pacman.position.x, pacman.position.y);
            
            // Reproducir sonido de fruta
            AudioManager::get().playSound(SoundID::Fruit);
            
            fruitVisible = false;
            fruitEaten = true;
            fruitRespawnTimer = 0.0f;
        }
    }
}

void Game::eatGhost(Ghost& ghost) {
    // Primero calcular puntos
    int points;
    switch (ghostsEatenInFright) {
        case 0: points = SCORE_GHOST_1; break;
        case 1: points = SCORE_GHOST_2; break;
        case 2: points = SCORE_GHOST_3; break;
        default: points = SCORE_GHOST_4; break;
    }
    
    score += points;
    checkHighScore();
    ghostsEatenInFright++;
    
    addFloatingScore(points, ghost.position.x, ghost.position.y);
    
    // Enviar fantasma a casa (modo Eyes)
    ghost.sendToHouse();
    
    // Detener sirena y waka durante el freeze
    AudioManager::get().stopSiren();
    AudioManager::get().stopSound(SoundID::Waka);
    
    // Reproducir sonido de comer fantasma DESPUÉS de detener otros
    AudioManager::get().playSound(SoundID::GhostEaten);
    
    // Reproducir b2b.wav inmediatamente (el fantasma ya está en modo Eyes)
    AudioManager::get().playSound(SoundID::BackToBase, -1);
    
    // Congelar el juego
    state = GameState::GhostEaten;
    freezeTimer = FREEZE_TIME;
}

void Game::pacmanDied() {
    // Detener todos los sonidos
    AudioManager::get().stopSiren();
    AudioManager::get().stopSound(SoundID::Waka);
    AudioManager::get().stopSound(SoundID::BackToBase);
    
    // Entrar en estado PreDeath (pausa de 1 segundo)
    state = GameState::PreDeath;
    freezeTimer = FREEZE_TIME;
}

void Game::checkLevelComplete() {
    if (Map::get().getRemainingDots() <= 0) {
        state = GameState::LevelClear;
        levelClearTimer = 0.0f;
        levelClearBlinkTimer = 0.0f;
        levelClearBlinkState = false;
        
        AudioManager::get().stopSiren();
        AudioManager::get().stopSound(SoundID::Waka);
        AudioManager::get().stopSound(SoundID::BackToBase);
    }
}

void Game::spawnFruit() {
    // La fruta aparece después de 70 y 170 dots comidos (primera aparición)
    if ((dotsEaten == 70 || dotsEaten == 170) && !fruitVisible && !fruitEaten) {
        fruitVisible = true;
        fruitTimer = FRUIT_VISIBLE_TIME;
    }
}

void Game::updateSiren() {
    int remaining = Map::get().getRemainingDots();
    bool shouldBeFast = remaining < 30;
    
    if (shouldBeFast != sirenFast && frightenedTimer <= 0.0f) {
        sirenFast = shouldBeFast;
        AudioManager::get().playSiren(sirenFast);
    }
}

void Game::addFloatingScore(int value, float x, float y) {
    FloatingScore fs;
    fs.value = value;
    fs.x = x;
    fs.y = y;
    fs.timer = FLOATING_SCORE_TIME;
    fs.active = true;
    floatingScores.push_back(fs);
}

void Game::updateFloatingScores(float dt) {
    for (auto& fs : floatingScores) {
        if (fs.active) {
            fs.timer -= dt;
            fs.y -= 20.0f * dt;
            
            if (fs.timer <= 0.0f) {
                fs.active = false;
            }
        }
    }
    
    floatingScores.erase(
        std::remove_if(floatingScores.begin(), floatingScores.end(),
            [](const FloatingScore& fs) { return !fs.active; }),
        floatingScores.end()
    );
}

void Game::renderFloatingScores() {
    SDL_Renderer* sdlRenderer = renderer.getSDLRenderer();
    
    for (const auto& fs : floatingScores) {
        if (!fs.active) continue;
        
        std::string scoreText = std::to_string(fs.value);
        
        int x = static_cast<int>(fs.x);
        int y = static_cast<int>(fs.y) + GAME_OFFSET_Y;
        
        int digitWidth = 4 * SCALE;
        int totalWidth = static_cast<int>(scoreText.length()) * digitWidth;
        
        x -= totalWidth / 2;
        y -= 4 * SCALE;
        
        SDL_SetRenderDrawColor(sdlRenderer, 0, 255, 255, 255);
        
        for (size_t i = 0; i < scoreText.length(); i++) {
            char c = scoreText[i];
            int dx = x + static_cast<int>(i) * digitWidth;
            drawDigitSimple(sdlRenderer, c, dx, y, SCALE);
        }
    }
}

void Game::drawPausedText() {
    auto& tm = TextureManager::get();
    SDL_Texture* pauseTex = tm.getTexture("text_pause");
    
    if (pauseTex) {
        int w, h;
        SDL_QueryTexture(pauseTex, nullptr, nullptr, &w, &h);
        tm.draw("text_pause", 
            SCREEN_WIDTH / 2 - (w * SCALE) / 2,
            SCREEN_HEIGHT / 2 - (h * SCALE) / 2,
            w * SCALE, h * SCALE);
    }
}

void Game::renderHUD() {
    int hudY = SCREEN_HEIGHT - SCALED_TILE - 4;
    
    auto& tm = TextureManager::get();
    
    // Vidas
    for (int i = 0; i < lives - 1; i++) {
        int x = SCALED_TILE + i * (SCALED_TILE + 4);
        tm.draw("pacman_life", x, hudY, SCALED_TILE, SCALED_TILE);
    }
    
    // Frutas
    renderFruitDisplay();
}

void Game::renderFruitDisplay() {
    auto& tm = TextureManager::get();
    int hudY = SCREEN_HEIGHT - SCALED_TILE - 4;
    
    std::vector<std::string> fruitsToShow;
    
    if (level >= 1) fruitsToShow.push_back("fruit_cherry");
    if (level >= 2) fruitsToShow.push_back("fruit_strawberry");
    if (level >= 3) fruitsToShow.push_back("fruit_orange");
    if (level >= 4) fruitsToShow.push_back("fruit_orange");
    if (level >= 5) fruitsToShow.push_back("fruit_apple");
    
    if (fruitsToShow.size() > 5) {
        fruitsToShow.erase(fruitsToShow.begin(), fruitsToShow.begin() + (fruitsToShow.size() - 5));
    }
    
    int startX = SCREEN_WIDTH - SCALED_TILE - 4;
    for (int i = static_cast<int>(fruitsToShow.size()) - 1; i >= 0; i--) {
        int x = startX - (static_cast<int>(fruitsToShow.size()) - 1 - i) * (SCALED_TILE + 4);
        tm.draw(fruitsToShow[i], x, hudY, SCALED_TILE, SCALED_TILE);
    }
}

void Game::render() {
    renderer.clear();
    
    // En Level Clear, renderizar animación de parpadeo
    if (state == GameState::LevelClear) {
        renderLevelClearAnimation();
    }
    else {
        renderer.drawMaze();
        renderer.drawDots();
    }
    
    // Fruta en el mapa
    if (fruitVisible && state != GameState::LevelClear) {
        FruitInfo fruitInfo = getCurrentFruitInfo();
        TextureManager::get().draw(
            fruitInfo.textureKey,
            13 * SCALED_TILE,
            17 * SCALED_TILE + GAME_OFFSET_Y,
            SCALED_TILE,
            SCALED_TILE
        );
    }
    
    // Pac-Man
    bool showPacman = (state != GameState::PressStart && 
                       state != GameState::LevelClear);
    
    if (showPacman) {
        if (state != GameState::Death || !pacman.isDeathAnimationComplete()) {
            std::string pacTexture;
            double angle = 0;
            
            if (state == GameState::Death) {
                pacTexture = "pacman_death_" + std::to_string(pacman.getDeathFrame());
            }
            else {
                pacTexture = "pacman_" + std::to_string(pacman.getAnimFrame());
                
                switch (pacman.direction) {
                    case Direction::Right: angle = 0;   break;
                    case Direction::Down:  angle = 90;  break;
                    case Direction::Left:  angle = 180; break;
                    case Direction::Up:    angle = 270; break;
                    default: break;
                }
            }
            
            TextureManager::get().draw(
                pacTexture,
                static_cast<int>(pacman.position.x),
                static_cast<int>(pacman.position.y) + GAME_OFFSET_Y,
                SCALED_TILE,
                SCALED_TILE,
                angle
            );
        }
    }
    
    // Fantasmas
    bool showGhosts = (state != GameState::Death && 
                       state != GameState::PreDeath &&
                       state != GameState::PressStart && 
                       state != GameState::LevelClear);
    
    if (showGhosts) {
        for (auto& ghost : ghosts) {
            ghost.render(renderer.getSDLRenderer());
        }
    }
    
    // Puntajes flotantes
    renderFloatingScores();
    
    // HUD superior (score)
    bool shouldBlinkScore = (highScoreBlinkTimer > 0.0f && highScoreBlinkState);
    bool shouldBlinkHighScoreReset = (highScoreResetBlinkTimer > 0.0f && highScoreResetBlinkState);
    
    renderer.drawScore(score, highScore, lives, shouldBlinkScore || shouldBlinkHighScoreReset);
    
    // HUD inferior
    renderHUD();
    
    // Textos según estado - centrados dinámicamente
    auto& tm = TextureManager::get();
    
    if (state == GameState::PressStart) {
        if (blinkState) {
            SDL_Texture* tex = tm.getTexture("text_pressstart");
            if (tex) {
                int w, h;
                SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
                int x = SCREEN_WIDTH / 2 - (w * SCALE) / 2;
                int y = 17 * SCALED_TILE + GAME_OFFSET_Y;
                renderer.drawText("text_pressstart", x, y);
            }
        }
    }
    else if (state == GameState::Ready) {
        SDL_Texture* tex = tm.getTexture("text_ready");
        if (tex) {
            int w, h;
            SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
            int x = SCREEN_WIDTH / 2 - (w * SCALE) / 2;
            int y = 17 * SCALED_TILE + GAME_OFFSET_Y;
            renderer.drawText("text_ready", x, y);
        }
    }
    else if (state == GameState::GameOver) {
        SDL_Texture* tex = tm.getTexture("text_gameover");
        if (tex) {
            int w, h;
            SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
            int x = SCREEN_WIDTH / 2 - (w * SCALE) / 2;
            int y = 17 * SCALED_TILE + GAME_OFFSET_Y;
            renderer.drawText("text_gameover", x, y);
        }
    }
    else if (state == GameState::LevelClear) {
        SDL_Texture* tex = tm.getTexture("text_clear");
        if (tex) {
            int w, h;
            SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
            int x = SCREEN_WIDTH / 2 - (w * SCALE) / 2;
            int y = 17 * SCALED_TILE + GAME_OFFSET_Y;
            renderer.drawText("text_clear", x, y);
        }
    }
    else if (state == GameState::Paused) {
        drawPausedText();
    }
    
    renderer.present();
}

void Game::renderLevelClearAnimation() {
    renderer.drawMazeFlashing(levelClearBlinkState);
}

// Función para dibujar dígitos simples
static void drawDigitSimple(SDL_Renderer* renderer, char c, int x, int y, int scale) {
    static const uint8_t digitPatterns[10][5] = {
        {0b111, 0b101, 0b101, 0b101, 0b111}, // 0
        {0b010, 0b110, 0b010, 0b010, 0b111}, // 1
        {0b111, 0b001, 0b111, 0b100, 0b111}, // 2
        {0b111, 0b001, 0b111, 0b001, 0b111}, // 3
        {0b101, 0b101, 0b111, 0b001, 0b001}, // 4
        {0b111, 0b100, 0b111, 0b001, 0b111}, // 5
        {0b111, 0b100, 0b111, 0b101, 0b111}, // 6
        {0b111, 0b001, 0b010, 0b010, 0b010}, // 7
        {0b111, 0b101, 0b111, 0b101, 0b111}, // 8
        {0b111, 0b101, 0b111, 0b001, 0b111}, // 9
    };
    
    if (c < '0' || c > '9') return;
    
    const uint8_t* pattern = digitPatterns[c - '0'];
    
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 3; col++) {
            if (pattern[row] & (1 << (2 - col))) {
                SDL_Rect rect = {
                    x + col * scale,
                    y + row * scale,
                    scale,
                    scale
                };
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}
