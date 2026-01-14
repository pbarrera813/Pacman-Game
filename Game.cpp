// Game.cpp - Pac-Man Versión 3.0
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

// Windows: para guardar highscore en AppData
#ifdef _WIN32
#include <shlobj.h>
#include <direct.h>
#endif

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
    
    loadHighScore();
    previousHighScore = highScore;
    
    ghosts.push_back(Ghost(GhostType::Blinky));
    ghosts.push_back(Ghost(GhostType::Pinky));
    ghosts.push_back(Ghost(GhostType::Inky));
    ghosts.push_back(Ghost(GhostType::Clyde));
    
    // Inicializar área del icono de volumen
    volumeIconRect = {0, 0, 0, 0};
    
    state = GameState::PressStart;
    
    return true;
}

std::string Game::getHighScorePath() const {
#ifdef _WIN32
    char appDataPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath))) {
        std::string path = std::string(appDataPath) + "\\PacMan";
        _mkdir(path.c_str());  // Crear carpeta si no existe
        return path + "\\highscore.dat";
    }
#endif
    return "highscore.dat";  // Fallback para Linux/Mac o si falla
}

void Game::loadHighScore() {
    std::ifstream file(getHighScorePath(), std::ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(&highScore), sizeof(highScore));
        file.close();
    } else {
        highScore = 0;
    }
}

void Game::saveHighScore() {
    std::ofstream file(getHighScorePath(), std::ios::binary);
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
    
    highScoreResetBlinkTimer = 1.5f;
    AudioManager::get().playSound(SoundID::Success);
}

void Game::loadAllTextures() {
    auto& tm = TextureManager::get();
    
    // Pac-Man (movimiento)
    tm.load("pacman_0", "assets/gfx/pacman/pac_man_0.png");
    tm.load("pacman_1", "assets/gfx/pacman/pac_man_1.png");
    tm.load("pacman_2", "assets/gfx/pacman/pac_man_2.png");
    tm.load("pacman_3", "assets/gfx/pacman/pac_man_3.png");
    tm.load("pacman_4", "assets/gfx/pacman/pac_man_4.png");
    
    // Pac-Man (muerte - 12 frames)
    for (int i = 0; i < 12; i++) {
        std::string key = "pacman_death_" + std::to_string(i);
        std::string path = "assets/gfx/pacman_death/pacdeath_" + std::to_string(i) + ".png";
        tm.load(key, path);
    }
    
    // Contador de vidas
    tm.load("pacman_life", "assets/gfx/pacman_counter/lifecounter_0.png");
    
    // Fantasmas con direcciones (4 fantasmas x 4 direcciones x 2 frames = 32 sprites)
    const char* colors[] = {"red", "pink", "blue", "orange"};
    const char* dirs[] = {"up", "down", "left", "right"};
    
    for (const char* color : colors) {
        for (const char* dir : dirs) {
            for (int frame = 0; frame < 2; frame++) {
                std::string key = std::string("ghost_") + color + "_" + dir + "_" + std::to_string(frame);
                std::string path = std::string("assets/gfx/ghost/") + color + "_ghost/ghost_" + color + "_" + dir + "_" + std::to_string(frame) + ".png";
                tm.load(key, path);
            }
        }
    }
    
    // Fantasmas asustados (4 frames: 0-1 azul, 2-3 blanco para parpadeo)
    tm.load("ghost_afraid_0", "assets/gfx/ghost/ghost_afraid/afraid_0.png");
    tm.load("ghost_afraid_1", "assets/gfx/ghost/ghost_afraid/afraid_1.png");
    tm.load("ghost_afraid_2", "assets/gfx/ghost/ghost_afraid/afraid_2.png");
    tm.load("ghost_afraid_3", "assets/gfx/ghost/ghost_afraid/afraid_3.png");
    
    // Ojos de fantasmas
    tm.load("ghost_eyes_up", "assets/gfx/ghost/eyes/eyes_up.png");
    tm.load("ghost_eyes_down", "assets/gfx/ghost/eyes/eyes_down.png");
    tm.load("ghost_eyes_left", "assets/gfx/ghost/eyes/eyes_left.png");
    tm.load("ghost_eyes_right", "assets/gfx/ghost/eyes/eyes_right.png");
    
    // Pills
    tm.load("pill", "assets/gfx/pill/pill_0.png");
    tm.load("super_pill", "assets/gfx/pill/pill_1.png");
    
    // Frutas (8 tipos)
    tm.load("fruit_cherry", "assets/gfx/fruits/spr_cherry_0.png");
    tm.load("fruit_strawberry", "assets/gfx/fruits/spr_strawberry_0.png");
    tm.load("fruit_orange", "assets/gfx/fruits/spr_orange_0.png");
    tm.load("fruit_apple", "assets/gfx/fruits/spr_apple_0.png");
    tm.load("fruit_melon", "assets/gfx/fruits/spr_melon_0.png");
    tm.load("fruit_ship", "assets/gfx/fruits/spr_ship_0.png");
    tm.load("fruit_bell", "assets/gfx/fruits/spr_bell_0.png");
    tm.load("fruit_key", "assets/gfx/fruits/spr_key_0.png");
    
    // Sprites de puntaje de frutas
    tm.load("points_fruit_100", "assets/gfx/points/fruits/first_fruit.png");
    tm.load("points_fruit_300", "assets/gfx/points/fruits/second_fruit.png");
    tm.load("points_fruit_500", "assets/gfx/points/fruits/third_fruit.png");
    tm.load("points_fruit_700", "assets/gfx/points/fruits/fourth_fruit.png");
    tm.load("points_fruit_1000", "assets/gfx/points/fruits/fifth_fruit.png");
    tm.load("points_fruit_2000", "assets/gfx/points/fruits/sixth_fruit.png");
    tm.load("points_fruit_3000", "assets/gfx/points/fruits/seventh_fruit.png");
    tm.load("points_fruit_5000", "assets/gfx/points/fruits/eight_fruit.png");
    
    // Sprites de puntaje de fantasmas
    tm.load("points_ghost_200", "assets/gfx/points/ghosts/first_ghost.png");
    tm.load("points_ghost_400", "assets/gfx/points/ghosts/second_ghost.png");
    tm.load("points_ghost_800", "assets/gfx/points/ghosts/third_ghost.png");
    tm.load("points_ghost_1600", "assets/gfx/points/ghosts/fourth_ghost.png");
    
    // Textos
    tm.load("text_ready", "assets/gfx/pacman_text/ready.png");
    tm.load("text_gameover", "assets/gfx/pacman_text/game_over.png");
    tm.load("text_clear", "assets/gfx/pacman_text/clear.png");
    tm.load("text_pressstart", "assets/gfx/pacman_text/enter_0.png");
    tm.load("text_pause", "assets/gfx/pacman_text/pause.png");
    
    // Iconos de volumen
    tm.load("volume_100", "assets/gfx/volume/sound_100.png");
    tm.load("volume_50", "assets/gfx/volume/sound_50.png");
    tm.load("volume_25", "assets/gfx/volume/sound_25.png");
    tm.load("volume_0", "assets/gfx/volume/no_sound.png");
}

FruitInfo Game::getCurrentFruitInfo() const {
    FruitInfo info;
    
    switch (level) {
        case 1:
            info.type = FruitType::Cherry;
            info.textureKey = "fruit_cherry";
            info.pointsTextureKey = "points_fruit_100";
            info.points = 100;
            break;
        case 2:
            info.type = FruitType::Strawberry;
            info.textureKey = "fruit_strawberry";
            info.pointsTextureKey = "points_fruit_300";
            info.points = 300;
            break;
        case 3:
        case 4:
            info.type = FruitType::Orange;
            info.textureKey = "fruit_orange";
            info.pointsTextureKey = "points_fruit_500";
            info.points = 500;
            break;
        case 5:
        case 6:
            info.type = FruitType::Apple;
            info.textureKey = "fruit_apple";
            info.pointsTextureKey = "points_fruit_700";
            info.points = 700;
            break;
        case 7:
        case 8:
            info.type = FruitType::Melon;
            info.textureKey = "fruit_melon";
            info.pointsTextureKey = "points_fruit_1000";
            info.points = 1000;
            break;
        case 9:
        case 10:
            info.type = FruitType::Ship;
            info.textureKey = "fruit_ship";
            info.pointsTextureKey = "points_fruit_2000";
            info.points = 2000;
            break;
        case 11:
        case 12:
            info.type = FruitType::Bell;
            info.textureKey = "fruit_bell";
            info.pointsTextureKey = "points_fruit_3000";
            info.points = 3000;
            break;
        default:  // Nivel 13+
            info.type = FruitType::Key;
            info.textureKey = "fruit_key";
            info.pointsTextureKey = "points_fruit_5000";
            info.points = 5000;
            break;
    }
    
    return info;
}

std::string Game::getGhostPointsTexture(int ghostIndex) const {
    switch (ghostIndex) {
        case 0: return "points_ghost_200";
        case 1: return "points_ghost_400";
        case 2: return "points_ghost_800";
        default: return "points_ghost_1600";
    }
}

std::string Game::getFruitTextureForLevel(int lvl) const {
    if (lvl == 1) return "fruit_cherry";
    if (lvl == 2) return "fruit_strawberry";
    if (lvl <= 4) return "fruit_orange";
    if (lvl <= 6) return "fruit_apple";
    if (lvl <= 8) return "fruit_melon";
    if (lvl <= 10) return "fruit_ship";
    if (lvl <= 12) return "fruit_bell";
    return "fruit_key";
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
                        previousHighScore = highScore;
                        highScoreBeaten = false;
                        highScoreBlinkTimer = 0.0f;
                        highScoreBlinkAccum = 0.0f;
                        highScoreBlinkState = false;
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
                        // Detener todos los sonidos de juego
                        AudioManager::get().stopSiren();
                        AudioManager::get().stopSound(SoundID::Waka);
                        AudioManager::get().stopSound(SoundID::PowerUp);
                        AudioManager::get().stopSound(SoundID::BackToBase);
                        wakaPlaying = false;
                        AudioManager::get().playSound(SoundID::Pause);
                    }
                    else if (state == GameState::Paused) {
                        state = stateBeforePause;
                        AudioManager::get().playSound(SoundID::Unpause);
                        // Reanudar sonido apropiado según estado
                        if (frightenedTimer > 0.0f) {
                            AudioManager::get().playSound(SoundID::PowerUp, -1);
                        } else {
                            AudioManager::get().playSiren(sirenFast);
                        }
                    }
                    else if (state == GameState::PressStart) {
                        running = false;
                    }
                    break;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;
                handleVolumeClick(mouseX, mouseY);
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
    
    // Reset waka state
    wakaTimer = 0.0f;
    wakaPlaying = false;
    
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
    blinkTimer += dt;
    if (blinkTimer >= 0.3f) {
        blinkTimer = 0.0f;
        blinkState = !blinkState;
    }
    
    updateHighScoreBlink(dt);
    
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
            freezeTimer -= dt;
            if (freezeTimer <= 0.0f) {
                state = GameState::Playing;
                if (frightenedTimer <= 0.0f) {
                    AudioManager::get().playSiren(sirenFast);
                }
            }
            updateFloatingScores(dt);
            break;
            
        case GameState::PreDeath:
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
        
        if (collectedFruits.size() >= 7) {
            collectedFruits.erase(collectedFruits.begin());
        }
        FruitInfo fruitInfo = getCurrentFruitInfo();
        collectedFruits.push_back(fruitInfo.type);
        
        Map::get().resetLevel();
        startLevel();
    }
}

void Game::updatePlaying(float dt) {
    updateScatterChaseMode(dt);
    
    pacman.update(dt);
    
    // Manejo de sonido waka con timer
    if (pacman.ateDot || pacman.atePowerPellet) {
        wakaTimer = WAKA_TIMEOUT;  // Reiniciar timer
        if (!wakaPlaying) {
            AudioManager::get().playSound(SoundID::Waka, -1);  // Loop
            wakaPlaying = true;
        }
    } else {
        // Decrementar timer si no está comiendo
        if (wakaPlaying) {
            wakaTimer -= dt;
            if (wakaTimer <= 0.0f) {
                AudioManager::get().stopSound(SoundID::Waka);
                wakaPlaying = false;
            }
        }
    }
    
    if (pacman.ateDot) {
        score += SCORE_DOT;
        dotsEaten++;
        checkHighScore();
        spawnFruit();
    }
    
    if (pacman.atePowerPellet) {
        score += SCORE_POWER_PELLET;
        dotsEaten++;
        checkHighScore();
        activateFrightenedMode();
        spawnFruit();
    }
    
    updateFloatingScores(dt);
    
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
            // Detener PowerUp y reanudar sirena
            AudioManager::get().stopSound(SoundID::PowerUp);
            AudioManager::get().playSiren(sirenFast);
        }
    }
    
    bool anyEyes = false;
    for (auto& ghost : ghosts) {
        if (ghost.getMode() == GhostMode::Eyes) {
            anyEyes = true;
            break;
        }
    }
    
    if (anyEyes) {
        if (!AudioManager::get().isPlaying(SoundID::BackToBase)) {
            AudioManager::get().playSound(SoundID::BackToBase, -1);
        }
    } else {
        AudioManager::get().stopSound(SoundID::BackToBase);
    }
    
    Ghost* blinky = &ghosts[0];
    for (auto& ghost : ghosts) {
        Vector2 target = GhostAI::getTarget(ghost, pacman, blinky);
        ghost.setTarget(target);
        ghost.update(dt);
    }
    
    checkCollisions();
    updateSiren();
    checkLevelComplete();
    
    if (fruitVisible) {
        fruitTimer -= dt;
        if (fruitTimer <= 0.0f) {
            fruitVisible = false;
        }
    }
    
    if (fruitEaten && !fruitVisible) {
        fruitRespawnTimer += dt;
        if (fruitRespawnTimer >= FRUIT_RESPAWN_TIME) {
            fruitVisible = true;
            fruitTimer = FRUIT_VISIBLE_TIME;
            fruitRespawnTimer = 0.0f;
            fruitEaten = false;
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
    
    // Detener sirena y reproducir PowerUp en loop
    AudioManager::get().stopSiren();
    AudioManager::get().playSound(SoundID::PowerUp, -1);  // -1 = loop infinito
}

void Game::checkCollisions() {
    int pacTileX = pacman.getTileX();
    int pacTileY = pacman.getTileY();
    
    for (auto& ghost : ghosts) {
        int ghostTileX = ghost.getTileX();
        int ghostTileY = ghost.getTileY();
        
        if (pacTileX == ghostTileX && pacTileY == ghostTileY) {
            if (ghost.getMode() == GhostMode::Frightened) {
                eatGhost(ghost);
                return;
            }
            else if (ghost.getMode() != GhostMode::Eyes) {
                pacmanDied();
                return;
            }
        }
    }
    
    if (fruitVisible) {
        float fruitX = 13.5f * SCALED_TILE;
        float fruitY = 17.0f * SCALED_TILE;
        
        float pacCenterX = pacman.position.x + SCALED_TILE / 2.0f;
        float pacCenterY = pacman.position.y + SCALED_TILE / 2.0f;
        
        float dx = pacCenterX - fruitX;
        float dy = pacCenterY - fruitY;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance < SCALED_TILE * 0.6f) {
            FruitInfo fruitInfo = getCurrentFruitInfo();
            score += fruitInfo.points;
            checkHighScore();
            
            // Usar sprite de puntaje
            addFloatingScore(fruitInfo.pointsTextureKey, pacman.position.x, pacman.position.y);
            
            AudioManager::get().playSound(SoundID::Fruit);
            
            fruitVisible = false;
            fruitEaten = true;
            fruitRespawnTimer = 0.0f;
        }
    }
}

void Game::eatGhost(Ghost& ghost) {
    int points;
    std::string pointsTexture;
    
    switch (ghostsEatenInFright) {
        case 0: 
            points = SCORE_GHOST_1; 
            pointsTexture = "points_ghost_200";
            break;
        case 1: 
            points = SCORE_GHOST_2; 
            pointsTexture = "points_ghost_400";
            break;
        case 2: 
            points = SCORE_GHOST_3; 
            pointsTexture = "points_ghost_800";
            break;
        default: 
            points = SCORE_GHOST_4; 
            pointsTexture = "points_ghost_1600";
            break;
    }
    
    score += points;
    checkHighScore();
    ghostsEatenInFright++;
    
    // Usar sprite de puntaje
    addFloatingScore(pointsTexture, ghost.position.x, ghost.position.y);
    
    ghost.sendToHouse();
    
    // Detener waka pero NO PowerUp (debe seguir sonando)
    AudioManager::get().stopSound(SoundID::Waka);
    wakaPlaying = false;
    
    AudioManager::get().playSound(SoundID::GhostEaten);
    AudioManager::get().playSound(SoundID::BackToBase, -1);
    
    state = GameState::GhostEaten;
    freezeTimer = FREEZE_TIME;
}

void Game::pacmanDied() {
    AudioManager::get().stopSiren();
    AudioManager::get().stopSound(SoundID::Waka);
    AudioManager::get().stopSound(SoundID::PowerUp);
    AudioManager::get().stopSound(SoundID::BackToBase);
    wakaPlaying = false;
    
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
        AudioManager::get().stopSound(SoundID::PowerUp);
        AudioManager::get().stopSound(SoundID::BackToBase);
        wakaPlaying = false;
    }
}

void Game::spawnFruit() {
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

void Game::addFloatingScore(const std::string& textureKey, float x, float y) {
    FloatingScore fs;
    fs.textureKey = textureKey;
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
    auto& tm = TextureManager::get();
    
    for (const auto& fs : floatingScores) {
        if (!fs.active) continue;
        
        SDL_Texture* tex = tm.getTexture(fs.textureKey);
        if (tex) {
            int w, h;
            SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
            
            // Escalar el sprite de puntaje
            int drawW = w * SCALE;
            int drawH = h * SCALE;
            
            int x = static_cast<int>(fs.x) - drawW / 2 + SCALED_TILE / 2;
            int y = static_cast<int>(fs.y) + GAME_OFFSET_Y - drawH / 2;
            
            tm.draw(fs.textureKey, x, y, drawW, drawH);
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
    
    for (int i = 0; i < lives - 1; i++) {
        int x = SCALED_TILE + i * (SCALED_TILE + 4);
        tm.draw("pacman_life", x, hudY, SCALED_TILE, SCALED_TILE);
    }
    
    renderFruitDisplay();
}

void Game::renderFruitDisplay() {
    auto& tm = TextureManager::get();
    int hudY = SCREEN_HEIGHT - SCALED_TILE - 4;
    
    std::vector<std::string> fruitsToShow;
    
    // Mostrar frutas según nivel alcanzado (máximo 7 frutas visibles)
    for (int lvl = 1; lvl <= level && fruitsToShow.size() < 7; lvl++) {
        fruitsToShow.push_back(getFruitTextureForLevel(lvl));
    }
    
    // Dibujar de derecha a izquierda
    int startX = SCREEN_WIDTH - SCALED_TILE - 4;
    for (int i = static_cast<int>(fruitsToShow.size()) - 1; i >= 0; i--) {
        int x = startX - (static_cast<int>(fruitsToShow.size()) - 1 - i) * (SCALED_TILE + 4);
        tm.draw(fruitsToShow[i], x, hudY, SCALED_TILE, SCALED_TILE);
    }
}

void Game::render() {
    renderer.clear();
    
    if (state == GameState::LevelClear) {
        renderLevelClearAnimation();
    }
    else {
        renderer.drawMaze();
        renderer.drawDots();
    }
    
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
    
    bool showGhosts = (state != GameState::Death && 
                       state != GameState::PreDeath &&
                       state != GameState::PressStart && 
                       state != GameState::LevelClear);
    
    if (showGhosts) {
        for (auto& ghost : ghosts) {
            ghost.render(renderer.getSDLRenderer());
        }
    }
    
    renderFloatingScores();
    
    bool shouldBlinkScore = (highScoreBlinkTimer > 0.0f && highScoreBlinkState);
    bool shouldBlinkHighScoreReset = (highScoreResetBlinkTimer > 0.0f && highScoreResetBlinkState);
    
    renderer.drawScore(score, highScore, lives, shouldBlinkScore || shouldBlinkHighScoreReset);
    
    renderHUD();
    renderVolumeIcon();
    
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

// ===== CONTROL DE VOLUMEN =====

std::string Game::getVolumeTextureKey() const {
    switch (volumeLevel) {
        case 100: return "volume_100";
        case 50:  return "volume_50";
        case 25:  return "volume_25";
        case 0:   return "volume_0";
        default:  return "volume_100";
    }
}

void Game::cycleVolume() {
    // Ciclo: 100 -> 50 -> 25 -> 0 -> 100
    switch (volumeLevel) {
        case 100: volumeLevel = 50;  break;
        case 50:  volumeLevel = 25;  break;
        case 25:  volumeLevel = 0;   break;
        case 0:   volumeLevel = 100; break;
        default:  volumeLevel = 100; break;
    }
    
    // Aplicar nuevo volumen
    AudioManager::get().setVolume(volumeLevel);
    
    // Reproducir sonido de confirmación (se escuchará al nuevo volumen)
    if (volumeLevel > 0) {
        AudioManager::get().playSound(SoundID::Success);
    }
}

void Game::handleVolumeClick(int mouseX, int mouseY) {
    // Verificar si el clic está dentro del área del icono de volumen
    if (mouseX >= volumeIconRect.x && mouseX <= volumeIconRect.x + volumeIconRect.w &&
        mouseY >= volumeIconRect.y && mouseY <= volumeIconRect.y + volumeIconRect.h) {
        cycleVolume();
    }
}

void Game::renderVolumeIcon() {
    auto& tm = TextureManager::get();
    std::string texKey = getVolumeTextureKey();
    
    SDL_Texture* tex = tm.getTexture(texKey);
    if (tex) {
        int w, h;
        SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
        
        // Escalar el icono según el tipo (50% para sonido, 25% para mudo)
        float scale = (volumeLevel == 0) ? 0.5f : 1.0f;
        int iconW = static_cast<int>(w * scale);
        int iconH = static_cast<int>(h * scale);
        
        // Posición: esquina superior derecha
        int iconX = SCREEN_WIDTH - iconW - 8;
        int iconY = 8;
        
        // Actualizar el rectángulo clickeable
        volumeIconRect = {iconX, iconY, iconW, iconH};
        
        tm.draw(texKey, iconX, iconY, iconW, iconH);
    }
}
