#pragma once
#include <map>
#include <string>
#include <vector>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics/Font.hpp>
#include "Animation.h"

class Assets
{
    std::map<std::string, sf::Texture> textures_;
    std::map<std::string, Animation> animations_;
    std::map<std::string, sf::Sound> sounds_;
    std::map<std::string, sf::SoundBuffer> soundBuffers_;
    std::map<std::string, sf::Font> fonts_;
    std::map<std::string, std::string> fontPaths_;
    Assets() = default;
public:
    
    static Assets& getInstance();
    void addTexture(std::string name, std::string path);
    void addSound(std::string name, std::string path);
    void addAnimation(std::string name, Animation animation);
    void addFont(std::string name, std::string path);
    
    sf::Texture& getTexture(std::string name);
    sf::Sound& getSound(std::string name);
    sf::Font& getFont(std::string name);
    const std::string& getFontPath(const std::string& name) const;
    bool hasAnimation(const std::string& name) const;
    Animation& getAnimation(std::string name);

    std::vector<std::string> getTextureNames() const;
    std::vector<std::string> getAnimationNames() const;
    std::vector<std::string> getSoundNames() const;
    std::vector<std::string> getFontNames() const;
};
