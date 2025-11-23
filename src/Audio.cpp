#include "Audio.hpp"
#include <stdexcept>
#include <iostream>
#include <cstdlib>

AudioPlayer::~AudioPlayer() {
    if (m_beepSound)
        free(m_beepSound);
    Mix_CloseAudio();
}

void AudioPlayer::Init() {
    if (Mix_OpenAudio(44100, AUDIO_S16LSB, MIX_DEFAULT_CHANNELS, 2048) < 0)
        throw std::runtime_error("[ERROR] Unable to initialize SDL2 Mixer ! :D");
    
    
    m_beepSound = Mix_LoadWAV("beep_sound.wav");
    if (m_beepSound == nullptr)
        throw std::runtime_error("[ERROR] Memory Allocation Error !");
    
    Mix_VolumeChunk(m_beepSound, MIX_MAX_VOLUME / 2);
}

void AudioPlayer::PlayBeepSound() {
    int check = Mix_PlayChannel(-1, m_beepSound, 0);
    if (check < 0) {
        std::cerr << "[ERROR] Unable to play beep sound !\n";
        std::exit(EXIT_FAILURE);
    }
}
