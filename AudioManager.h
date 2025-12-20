// AudioManager.h
// Gestor de audio SDL2_mixer
#pragma once

#include <SDL2/SDL_mixer.h>
#include <string>
#include <unordered_map>

enum class SoundID {
    Startup,        // Inicio del juego
    Waka,           // Comer dot
    PowerUp,        // Comer power pellet
    GhostEaten,     // Comer fantasma
    BackToBase,     // Fantasma vuelve a casa
    Death,          // Pac-Man muere
    Fruit,          // Comer fruta
    Siren,          // Sirena normal
    SirenFast,      // Sirena rápida
    HighScore,      // Nuevo high score
    Pause           // Sonido de pausa
};

class AudioManager {
public:
    static AudioManager& get();
    
    bool init();
    void shutdown();
    
    // Reproducir sonido
    void playSound(SoundID id, int loops = 0);
    void stopSound(SoundID id);
    
    // Control de sirena
    void playSiren(bool fast);
    void stopSiren();
    
    // Detener todo
    void stopAll();
    
    // Verificar si hay sonido reproduciéndose
    bool isPlaying(SoundID id) const;
    bool isAnySoundPlaying() const;
    
private:
    AudioManager() = default;
    ~AudioManager() = default;
    
    bool loadSound(SoundID id, const std::string& path);
    
    std::unordered_map<SoundID, Mix_Chunk*> sounds;
    std::unordered_map<SoundID, int> activeChannels;
    
    static constexpr int WAKA_CHANNEL = 0;  // Canal dedicado para waka
    static constexpr int SIREN_CHANNEL = 1; // Canal dedicado para sirena
    
    bool sirenPlaying = false;
    bool sirenFast = false;
};
