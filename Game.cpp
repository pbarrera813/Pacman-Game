// Game.cpp
#include "Game.h"
#include "GhostAI.h"
#include "Map.h"
#include "TextureManager.h"
#include "Constants.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include <algorithm>

// Función para dibujar dígitos simples
static void drawDigitSimple(SDL_Renderer* renderer, char digit, int x, int y, int scale);

// Tiempos de Scatter/Chase
static const float SCATTER_TIMES[] = {7.0f, 7.0f, 5.0f, 5.0f};
static const float CHASE_TIMES[] = {20.0f, 20.0f, 20.0f, 999999.0f};

Game::Game() {}

Game::~Game() {
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
    
    // Crear fantasmas
    ghosts.push_back(Ghost(GhostType::Blinky));
    ghosts.push_back(Ghost(GhostType::Pinky));
    ghosts.push_back(Ghost(GhostType::Inky));
    ghosts.push_back(Ghost(GhostType::Clyde));
    
    // Empezar en pantalla de título
    state = GameState::PressStart;
    
    return true;
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
    
    // Pills (IMPORTANTE: usar los sprites)
    tm.load("pill", "assets/gfx/pill/pill_0.png");
    tm.load("super_pill", "assets/gfx/pill/pill_1.png");
    
    // Cherry
    tm.load("cherry", "assets/gfx/cherry/spr_cherry_0.png");
    
    // Textos
    tm.load("text_ready", "assets/gfx/pacman_text/ready.png");
    tm.load("text_gameover", "assets/gfx/pacman_text/game_over.png");
    tm.load("text_clear", "assets/gfx/pacman_text/clear.png");
    tm.load("text_pressstart", "assets/gfx/pacman_text/enter_0.png");
    tm.load("text_pause", "assets/gfx/pacman_text/pause.png");
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
                    
                case SDLK_RETURN:
                    if (state == GameState::PressStart) {
                        // Iniciar juego
                        state = GameState::Startup;
                        stateTimer = 0.0f;
                        AudioManager::get().playSound(SoundID::Startup);
                    }
                    else if (state == GameState::GameOver) {
                        // Reiniciar
                        score = 0;
                        lives = 3;
                        level = 1;
                        Map::get().resetLevel();
                        startLevel();
                    }
                    break;
                    
                case SDLK_ESCAPE:
                    if (state == GameState::Playing) {
                        // Pausar
                        stateBeforePause = state;
                        state = GameState::Paused;
                        AudioManager::get().playSound(SoundID::Pause);
                        AudioManager::get().stopSiren();
                    }
                    else if (state == GameState::Paused) {
                        // Despausar
                        state = stateBeforePause;
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
    for (auto& ghost : ghosts) {
        ghost.reset();
    }
}

void Game::update(float dt) {
    // Parpadeo para textos
    blinkTimer += dt;
    if (blinkTimer >= 0.3f) {
        blinkTimer = 0.0f;
        blinkState = !blinkState;
    }
    
    switch (state) {
        case GameState::PressStart:
            // Solo esperar input
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
            // No actualizar nada
            break;
            
        case GameState::Death:
            pacman.updateDeathAnimation(dt);
            if (pacman.isDeathAnimationComplete()) {
                lives--;
                if (lives <= 0) {
                    state = GameState::GameOver;
                }
                else {
                    resetPositions();
                    state = GameState::Ready;
                    stateTimer = READY_TIME;
                }
            }
            break;
            
        case GameState::LevelClear:
            stateTimer -= dt;
            if (stateTimer <= 0.0f) {
                level++;
                Map::get().resetLevel();
                startLevel();
            }
            break;
            
        case GameState::GameOver:
            // Esperar input
            break;
    }
}

void Game::updatePlaying(float dt) {
    updateScatterChaseMode(dt);
    
    // Guardar si estaba comiendo
    bool wasEating = pacman.ateDot || pacman.atePowerPellet;
    
    pacman.update(dt);
    
    // Sonido waka: reproducir si está comiendo
    if (pacman.ateDot || pacman.atePowerPellet) {
        AudioManager::get().playSound(SoundID::Waka);
    }
    
    // Puntuación por dot
    if (pacman.ateDot) {
        score += SCORE_DOT;
        dotsEaten++;
        spawnFruit();
    }
    
    // Power pellet
    if (pacman.atePowerPellet) {
        score += SCORE_POWER_PELLET;
        dotsEaten++;
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
    
    // Fruta timer
    if (fruitVisible) {
        fruitTimer -= dt;
        if (fruitTimer <= 0.0f) {
            fruitVisible = false;
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
                ghost.getMode() != GhostMode::Eyes &&
                !ghost.isInHouse()) {
                ghost.setMode(newMode);
                ghost.reverseDirection();
            }
        }
    }
}

void Game::activateFrightenedMode() {
    AudioManager::get().stopSiren();
    AudioManager::get().playSound(SoundID::PowerUp);
    
    frightenedTimer = FRIGHTENED_TIME;
    ghostsEatenInFright = 0;
    
    for (auto& ghost : ghosts) {
        if (ghost.getMode() != GhostMode::Eyes && !ghost.isInHouse()) {
            ghost.setMode(GhostMode::Frightened);
        }
    }
}

void Game::checkCollisions() {
    for (auto& ghost : ghosts) {
        if (ghost.isInHouse()) continue;
        
        float dist = pacman.position.distance(ghost.position);
        
        if (dist < SCALED_TILE * 0.7f) {
            if (ghost.getMode() == GhostMode::Frightened) {
                eatGhost(ghost);
            }
            else if (ghost.getMode() != GhostMode::Eyes) {
                pacmanDied();
                return;
            }
        }
    }
    
    // Colisión con fruta
    if (fruitVisible) {
        float fruitX = 13.5f * SCALED_TILE;
        float fruitY = 17 * SCALED_TILE;
        
        float dist = std::sqrt(
            std::pow(pacman.position.x - fruitX, 2) +
            std::pow(pacman.position.y - fruitY, 2)
        );
        
        if (dist < SCALED_TILE) {
            fruitVisible = false;
            fruitEaten = true;
            score += SCORE_CHERRY;
            addFloatingScore(SCORE_CHERRY, fruitX, fruitY);
            AudioManager::get().playSound(SoundID::Fruit);
        }
    }
}

void Game::eatGhost(Ghost& ghost) {
    ghostsEatenInFright++;
    
    int ghostScore = SCORE_GHOST_1 * (1 << (ghostsEatenInFright - 1));
    score += ghostScore;
    
    addFloatingScore(ghostScore, ghost.position.x, ghost.position.y);
    
    AudioManager::get().playSound(SoundID::GhostEaten);
    
    ghost.sendToHouse();
}

void Game::pacmanDied() {
    AudioManager::get().stopAll();
    AudioManager::get().playSound(SoundID::Death);
    
    pacman.die();
    state = GameState::Death;
}

void Game::checkLevelComplete() {
    if (Map::get().getRemainingDots() == 0) {
        AudioManager::get().stopAll();
        state = GameState::LevelClear;
        stateTimer = LEVEL_CLEAR_TIME;
    }
}

void Game::spawnFruit() {
    if (!fruitEaten && (dotsEaten == 70 || dotsEaten == 170)) {
        fruitVisible = true;
        fruitTimer = FRUIT_VISIBLE_TIME;
    }
}

void Game::updateSiren() {
    int remaining = Map::get().getRemainingDots();
    bool shouldBeFast = remaining < 40;
    
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
    SDL_Renderer* sdlRenderer = renderer.getSDLRenderer();
    
    // Intentar usar textura de pausa
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
    else {
        // Dibujar "PAUSED" con código
        const char* text = "PAUSED";
        int len = 6;
        int digitWidth = 4 * SCALE;
        int totalWidth = len * digitWidth;
        int x = SCREEN_WIDTH / 2 - totalWidth / 2;
        int y = SCREEN_HEIGHT / 2 - 3 * SCALE;
        
        SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 0, 255);
        
        // Dibujar cada letra
        for (int i = 0; i < len; i++) {
            drawDigitSimple(sdlRenderer, text[i], x + i * digitWidth, y, SCALE);
        }
    }
}

void Game::render() {
    renderer.clear();
    renderer.drawMaze();
    renderer.drawDots();
    
    // Fruta
    if (fruitVisible) {
        TextureManager::get().draw(
            "cherry",
            13 * SCALED_TILE,
            17 * SCALED_TILE + GAME_OFFSET_Y,
            SCALED_TILE,
            SCALED_TILE
        );
    }
    
    // Pac-Man
    if (state != GameState::PressStart) {
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
    if (state != GameState::Death && state != GameState::PressStart) {
        for (auto& ghost : ghosts) {
            ghost.render(renderer.getSDLRenderer());
        }
    }
    
    // Puntajes flotantes
    renderFloatingScores();
    
    // HUD
    renderer.drawScore(score, highScore, lives);
    renderer.drawLives(lives);
    
    // Textos según estado
    if (state == GameState::PressStart) {
        if (blinkState) {
            renderer.drawText("text_pressstart",
                SCREEN_WIDTH / 2 - 44 * SCALE,
                17 * SCALED_TILE + GAME_OFFSET_Y);
        }
    }
    else if (state == GameState::Ready) {
        renderer.drawText("text_ready", 
            SCREEN_WIDTH / 2 - 24 * SCALE, 
            17 * SCALED_TILE + GAME_OFFSET_Y);
    }
    else if (state == GameState::GameOver) {
        renderer.drawText("text_gameover",
            SCREEN_WIDTH / 2 - 40 * SCALE,
            17 * SCALED_TILE + GAME_OFFSET_Y);
    }
    else if (state == GameState::LevelClear) {
        renderer.drawText("text_clear",
            SCREEN_WIDTH / 2 - 24 * SCALE,
            17 * SCALED_TILE + GAME_OFFSET_Y);
    }
    else if (state == GameState::Paused) {
        drawPausedText();
    }
    
    renderer.present();
    
    if (score > highScore) {
        highScore = score;
    }
}

// Función para dibujar dígitos y letras simples
static void drawDigitSimple(SDL_Renderer* renderer, char c, int x, int y, int scale) {
    // Patrones 3x5 para dígitos
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
    
    // Patrones para letras PAUSED
    static const uint8_t letterPatterns[26][5] = {
        {0b010, 0b101, 0b111, 0b101, 0b101}, // A
        {0b110, 0b101, 0b110, 0b101, 0b110}, // B
        {0b011, 0b100, 0b100, 0b100, 0b011}, // C
        {0b110, 0b101, 0b101, 0b101, 0b110}, // D
        {0b111, 0b100, 0b110, 0b100, 0b111}, // E
        {0b111, 0b100, 0b110, 0b100, 0b100}, // F
        {0b011, 0b100, 0b101, 0b101, 0b011}, // G
        {0b101, 0b101, 0b111, 0b101, 0b101}, // H
        {0b111, 0b010, 0b010, 0b010, 0b111}, // I
        {0b001, 0b001, 0b001, 0b101, 0b010}, // J
        {0b101, 0b110, 0b100, 0b110, 0b101}, // K
        {0b100, 0b100, 0b100, 0b100, 0b111}, // L
        {0b101, 0b111, 0b111, 0b101, 0b101}, // M
        {0b101, 0b111, 0b111, 0b111, 0b101}, // N
        {0b010, 0b101, 0b101, 0b101, 0b010}, // O
        {0b110, 0b101, 0b110, 0b100, 0b100}, // P
        {0b010, 0b101, 0b101, 0b110, 0b011}, // Q
        {0b110, 0b101, 0b110, 0b101, 0b101}, // R
        {0b011, 0b100, 0b010, 0b001, 0b110}, // S
        {0b111, 0b010, 0b010, 0b010, 0b010}, // T
        {0b101, 0b101, 0b101, 0b101, 0b111}, // U
        {0b101, 0b101, 0b101, 0b101, 0b010}, // V
        {0b101, 0b101, 0b111, 0b111, 0b101}, // W
        {0b101, 0b101, 0b010, 0b101, 0b101}, // X
        {0b101, 0b101, 0b010, 0b010, 0b010}, // Y
        {0b111, 0b001, 0b010, 0b100, 0b111}, // Z
    };
    
    const uint8_t* pattern = nullptr;
    
    if (c >= '0' && c <= '9') {
        pattern = digitPatterns[c - '0'];
    }
    else if (c >= 'A' && c <= 'Z') {
        pattern = letterPatterns[c - 'A'];
    }
    else if (c >= 'a' && c <= 'z') {
        pattern = letterPatterns[c - 'a'];
    }
    
    if (!pattern) return;
    
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
