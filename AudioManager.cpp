#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager() {}

bool AudioManager::loadSound(const std::string& name, const std::string& path) {
    sf::SoundBuffer buf;
    if (!buf.loadFromFile(path)) {
        // Silently fail — sound just won't play
        return false;
    }
    buffers[name] = std::move(buf);
    sounds[name].setBuffer(buffers[name]);
    return true;
}

void AudioManager::loadAll() {
    loadSound("click",          "assets/sounds/click.wav");
    loadSound("transition",     "assets/sounds/transition.wav");
    loadSound("evil_laugh",     "assets/sounds/evil_laugh.wav");
    loadSound("husband_grunt",  "assets/sounds/husband_grunt.wav");
}

void AudioManager::play(const std::string& name) {
    if (muted) return;
    auto it = sounds.find(name);
    if (it != sounds.end()) {
        it->second.play();
    }
    // If sound not loaded, silently do nothing
}
