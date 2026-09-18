#pragma once

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <SFML/Audio.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>


#include "Animation.h"

class Assets {
    std::map<std::string, sf::Texture> textures_;
    std::map<std::string, Animation> animations_;
    std::map<std::string, sf::Sound> sounds_;
    std::map<std::string, sf::SoundBuffer> soundBuffers_;
    std::map<std::string, sf::Font> fonts_;
    std::map<std::string, std::string> fontPaths_;
    std::map<std::string, std::ifstream> prefabs_;

    Assets() = default;

  public:
    static Assets &getInstance();

    void addTexture(std::string name, std::string path);
    void addSound(std::string name, std::string path);
    void addAnimation(std::string name, Animation animation);
    void addFont(std::string name, std::string path);
    void addPrefabDefinition(std::string name, std::string filePath);

    sf::Texture &getTexture(const std::string &name);
    const sf::Texture &getTexture(const std::string &name) const;

    sf::Sound &getSound(const std::string &name);

    sf::Font &getFont(const std::string &name);
    const sf::Font &getFont(const std::string &name) const;

    const std::string &getFontPath(const std::string &name) const;

    Animation &getAnimation(const std::string &name);
    const Animation &getAnimation(const std::string &name) const;

    std::ifstream &getPrefabDefinition(const std::string &name);

    bool hasTexture(const std::string &name) const;
    bool hasSound(const std::string &name) const;
    bool hasFont(const std::string &name) const;
    bool hasAnimation(const std::string &name) const;
    bool hasPrefabDefinition(const std::string &name) const;

    std::vector<std::string> getTextureNames() const;
    std::vector<std::string> getAnimationNames() const;
    std::vector<std::string> getSoundNames() const;
    std::vector<std::string> getFontNames() const;
    std::vector<std::string> getPrefabDefinitions() const;
};