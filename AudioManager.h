#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <map>

class AudioManager {
public:
    AudioManager();

    // Load all sounds from assets/sounds/
    void loadAll();

    // Play a sound by name: "click", "transition", "evil_laugh", "husband_grunt"
    void play(const std::string& name);

    // Master mute toggle
    void setMuted(bool m) { muted = m; }
    bool isMuted() const  { return muted; }

private:
    // SoundBuffers must outlive Sounds
    std::map<std::string, sf::SoundBuffer> buffers;
    std::map<std::string, sf::Sound>       sounds;
    bool muted = false;

    bool loadSound(const std::string& name, const std::string& path);
};
