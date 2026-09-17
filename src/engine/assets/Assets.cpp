#include "Assets.h"
#include "Animation.h"
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

Assets& Assets::getInstance() {
    static Assets instance;
    return instance;
}

void Assets::addTexture(std::string name, std::string path)
{
    sf::Texture texture;
    if (texture.loadFromFile(path))
    {
        textures_[name] = texture;
    }
}

void Assets::addSound(std::string name, std::string path)
{
    sf::SoundBuffer buffer;
    if (buffer.loadFromFile(path))
    {
        soundBuffers_[name] = std::move(buffer);
        // Create the Sound only after the buffer is stored
        sounds_.emplace(name, soundBuffers_[name]); // constructs sf::Sound(buffer)
    }
}

void Assets::addAnimation(std::string name, Animation animation)
{
    animations_.insert_or_assign(std::move(name), std::move(animation));
}

void Assets::addFont(std::string name, std::string path)
{
    sf::Font font;
    if (font.openFromFile(path)) {
        fonts_[name] = font;
        fontPaths_[name] = path;
    }
}

void Assets::addPrefabDefinition(std::string name, std::string filepath) {
    prefabs_.emplace(name, std::ifstream(filepath));
    if (!prefabs_.at(name).is_open()) {
        prefabs_.erase(name);
        throw std::runtime_error("Failed to open asset file: " + filepath);
    }
}

sf::Texture& Assets::getTexture(std::string name)
{
    return textures_[name];
}

sf::Sound& Assets::getSound(std::string name)
{
    auto it = sounds_.find(name);
    if (it == sounds_.end())
    {
        throw std::runtime_error("Sound not found: " + name);
    }
    return it->second;
}

sf::Font& Assets::getFont(std::string name)
{
    return fonts_[name];
}

const std::string& Assets::getFontPath(const std::string& name) const
{
    auto it = fontPaths_.find(name);
    if (it == fontPaths_.end())
    {
        throw std::runtime_error("Font not found: " + name);
    }

    return it->second;
}

bool Assets::hasAnimation(const std::string& name) const
{
    return animations_.find(name) != animations_.end();
}

Animation& Assets::getAnimation(std::string name)
{
    auto it = animations_.find(name);
    if (it == animations_.end())
    {
        throw std::runtime_error("Animation not found: " + name);
    }

    return it->second;
}

std::vector<std::string> Assets::getTextureNames() const
{
    std::vector<std::string> names;
    for (const auto& [name, texture] : textures_) names.push_back(name);
    return names;
}

std::vector<std::string> Assets::getAnimationNames() const
{
    std::vector<std::string> names;
    for (const auto& [name, animation] : animations_) names.push_back(name);
    return names;
}

std::vector<std::string> Assets::getSoundNames() const
{
    std::vector<std::string> names;
    for (const auto& [name, sound] : sounds_) names.push_back(name);
    return names;
}

std::vector<std::string> Assets::getFontNames() const
{
    std::vector<std::string> names;
    for (const auto& [name, font] : fonts_) names.push_back(name);
    return names;
}

std::ifstream& Assets::getPrefabDefinition(std::string name) {
    return prefabs_[name];
}

std::vector<std::string> Assets::getPrefabDefinitions() const {
    std::vector<std::string> prefabsDef;
    for (const auto& [name, streams] : prefabs_) prefabsDef.push_back(name);
    return prefabsDef;
}

bool Assets::hasPrefabDefinition(std::string name) {
    return prefabs_.find(name) != prefabs_.end();
}
