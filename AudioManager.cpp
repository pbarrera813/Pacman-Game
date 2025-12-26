// AudioManager.cpp
#include "AudioManager.h"
#include <iostream>

AudioManager& AudioManager::get() {
    static AudioManager instance;
    return instance;
}

bool AudioManager::init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer init failed: " << Mix_GetError() << std::endl;
        return false;
    }
    
    // Reservar canales
    Mix_AllocateChannels(16);
    
    // Cargar todos los sonidos
    loadSound(SoundID::Startup,     "assets/sounds/startup.ogg");
    loadSound(SoundID::Waka,        "assets/sounds/eating.wav");
    loadSound(SoundID::PowerUp,     "assets/sounds/powerup.wav");
    loadSound(SoundID::GhostEaten,  "assets/sounds/ghost_eaten.wav");
    loadSound(SoundID::BackToBase,  "assets/sounds/b2b.wav");
    loadSound(SoundID::Death,       "assets/sounds/death.wav");
    loadSound(SoundID::Fruit,       "assets/sounds/fruit.wav");
    loadSound(SoundID::Siren,       "assets/sounds/siren.ogg");
    loadSound(SoundID::SirenFast,   "assets/sounds/siren2.ogg");
    loadSound(SoundID::HighScore,   "assets/sounds/high-score.ogg");
    loadSound(SoundID::Pause,       "assets/sounds/pause.wav");
    loadSound(SoundID::Unpause,     "assets/sounds/unpause.wav");
    loadSound(SoundID::Success,     "assets/sounds/success.wav");
    
    return true;
}

void AudioManager::shutdown() {
    stopAll();
    
    for (auto& pair : sounds) {
        if (pair.second) {
            Mix_FreeChunk(pair.second);
        }
    }
    sounds.clear();
    activeChannels.clear();
    
    Mix_CloseAudio();
}

bool AudioManager::loadSound(SoundID id, const std::string& path) {
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (!chunk) {
        std::cerr << "Warning: Failed to load sound: " << path << std::endl;
        return false;
    }
    sounds[id] = chunk;
    return true;
}

void AudioManager::playSound(SoundID id, int loops) {
    auto it = sounds.find(id);
    if (it == sounds.end() || !it->second)
        return;
    
    int channel = -1;
    
    // Waka usa canal dedicado
    if (id == SoundID::Waka) {
        if (!Mix_Playing(WAKA_CHANNEL)) {
            channel = Mix_PlayChannel(WAKA_CHANNEL, it->second, loops);
        }
        return;
    }
    
    // Sirenas usan canal dedicado
    if (id == SoundID::Siren || id == SoundID::SirenFast) {
        Mix_HaltChannel(SIREN_CHANNEL);
        channel = Mix_PlayChannel(SIREN_CHANNEL, it->second, loops);
        if (channel != -1) {
            activeChannels[id] = channel;
        }
        return;
    }
    
    // BackToBase usa canal dedicado
    if (id == SoundID::BackToBase) {
        if (!Mix_Playing(B2B_CHANNEL)) {
            channel = Mix_PlayChannel(B2B_CHANNEL, it->second, loops);
        }
        return;
    }
    
    // Fruta usa canal dedicado para evitar que se corte
    if (id == SoundID::Fruit) {
        Mix_HaltChannel(FRUIT_CHANNEL);  // Detener cualquier sonido anterior
        channel = Mix_PlayChannel(FRUIT_CHANNEL, it->second, loops);
        return;
    }
    
    // HighScore usa canal dedicado para asegurar reproducción
    if (id == SoundID::HighScore) {
        Mix_HaltChannel(HIGHSCORE_CHANNEL);  // Detener cualquier sonido anterior
        channel = Mix_PlayChannel(HIGHSCORE_CHANNEL, it->second, loops);
        return;
    }
    
    // Otros sonidos usan canales libres
    channel = Mix_PlayChannel(-1, it->second, loops);
    if (channel != -1) {
        activeChannels[id] = channel;
    }
}

void AudioManager::stopSound(SoundID id) {
    if (id == SoundID::Waka) {
        Mix_HaltChannel(WAKA_CHANNEL);
        return;
    }
    
    if (id == SoundID::Siren || id == SoundID::SirenFast) {
        Mix_HaltChannel(SIREN_CHANNEL);
        sirenPlaying = false;
        return;
    }
    
    if (id == SoundID::BackToBase) {
        Mix_HaltChannel(B2B_CHANNEL);
        return;
    }
    
    auto it = activeChannels.find(id);
    if (it != activeChannels.end()) {
        Mix_HaltChannel(it->second);
        activeChannels.erase(it);
    }
}

void AudioManager::playSiren(bool fast) {
    if (sirenPlaying && sirenFast == fast)
        return;
    
    stopSiren();
    
    sirenFast = fast;
    sirenPlaying = true;
    
    playSound(fast ? SoundID::SirenFast : SoundID::Siren, -1);
}

void AudioManager::stopSiren() {
    Mix_HaltChannel(SIREN_CHANNEL);
    sirenPlaying = false;
}

void AudioManager::stopAll() {
    Mix_HaltChannel(-1);
    activeChannels.clear();
    sirenPlaying = false;
}

bool AudioManager::isPlaying(SoundID id) const {
    if (id == SoundID::Waka) {
        return Mix_Playing(WAKA_CHANNEL) != 0;
    }
    if (id == SoundID::Siren || id == SoundID::SirenFast) {
        return Mix_Playing(SIREN_CHANNEL) != 0;
    }
    if (id == SoundID::BackToBase) {
        return Mix_Playing(B2B_CHANNEL) != 0;
    }
    
    auto it = activeChannels.find(id);
    if (it == activeChannels.end())
        return false;
    
    return Mix_Playing(it->second) != 0;
}

bool AudioManager::isAnySoundPlaying() const {
    return Mix_Playing(-1) > 0;
}
