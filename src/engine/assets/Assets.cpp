#include "Assets.h"
#include "Animation.h"

#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>


Assets& Assets::getInstance()
{
    static Assets instance;
    return instance;
}

namespace {

[[noreturn]] void throwAssetNotFound(const std::string& type, const std::string& name)
{
    throw std::runtime_error(type + " not found: \"" + name + "\"");
}

[[noreturn]] void throwLoadFailed(const std::string& type, const std::string& path)
{
    throw std::runtime_error("Failed to load " + type + " from: \"" + path + "\"");
}

void ensureNonEmptyName(const std::string& name, const std::string& context)
{
    if (name.empty())
        throw std::invalid_argument(context + ": asset name cannot be empty");
}

} // anonymous namespace 

void Assets::addTexture(std::string name, std::string path)
{
    ensureNonEmptyName(name, "addTexture");

    if (textures_.count(name))
        throw std::runtime_error("Texture already exists: \"" + name + "\"");

    sf::Texture texture;
    if (!texture.loadFromFile(path))
        throwLoadFailed("texture", path);

    textures_.emplace(std::move(name), std::move(texture));
}

void Assets::addSound(std::string name, std::string path)
{
    ensureNonEmptyName(name, "addSound");

    if (soundBuffers_.count(name) || sounds_.count(name))
        throw std::runtime_error("Sound already exists: \"" + name + "\"");

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(path))
        throwLoadFailed("sound", path);

    // Store the buffer first so the Sound can safely reference it
    auto [bufIt, inserted] = soundBuffers_.emplace(std::move(name), std::move(buffer));
    if (!inserted)
        throw std::runtime_error("Internal error: failed to store sound buffer");

    // Construct the Sound with a reference to the stored buffer
    sounds_.emplace(bufIt->first, bufIt->second);
}

void Assets::addAnimation(std::string name, Animation animation)
{
    ensureNonEmptyName(name, "addAnimation");

    if (animations_.count(name))
        throw std::runtime_error("Animation already exists: \"" + name + "\"");

    animations_.emplace(std::move(name), std::move(animation));
}

void Assets::addFont(std::string name, std::string path)
{
    ensureNonEmptyName(name, "addFont");

    if (fonts_.count(name))
        throw std::runtime_error("Font already exists: \"" + name + "\"");

    sf::Font font;
    if (!font.openFromFile(path))
        throwLoadFailed("font", path);

    fonts_.emplace(name, std::move(font));
    fontPaths_.emplace(std::move(name), std::move(path));
}

void Assets::addPrefabDefinition(std::string name, std::string filepath)
{
    ensureNonEmptyName(name, "addPrefabDefinition");

    if (prefabs_.count(name))
        throw std::runtime_error("Prefab definition already exists: \"" + name + "\"");

    std::ifstream stream(filepath);
    if (!stream.is_open())
        throwLoadFailed("prefab definition", filepath);

    prefabs_.emplace(std::move(name), std::move(stream));
}

sf::Texture& Assets::getTexture(const std::string& name)
{
    auto it = textures_.find(name);
    if (it == textures_.end())
        throwAssetNotFound("Texture", name);
    return it->second;
}

const sf::Texture& Assets::getTexture(const std::string& name) const
{
    auto it = textures_.find(name);
    if (it == textures_.end())
        throwAssetNotFound("Texture", name);
    return it->second;
}

sf::Sound& Assets::getSound(const std::string& name)
{
    auto it = sounds_.find(name);
    if (it == sounds_.end())
        throwAssetNotFound("Sound", name);
    return it->second;
}

sf::Font& Assets::getFont(const std::string& name)
{
    auto it = fonts_.find(name);
    if (it == fonts_.end())
        throwAssetNotFound("Font", name);
    return it->second;
}

const sf::Font& Assets::getFont(const std::string& name) const
{
    auto it = fonts_.find(name);
    if (it == fonts_.end())
        throwAssetNotFound("Font", name);
    return it->second;
}

const std::string& Assets::getFontPath(const std::string& name) const
{
    auto it = fontPaths_.find(name);
    if (it == fontPaths_.end())
        throwAssetNotFound("Font path", name);
    return it->second;
}

Animation& Assets::getAnimation(const std::string& name)
{
    auto it = animations_.find(name);
    if (it == animations_.end())
        throwAssetNotFound("Animation", name);
    return it->second;
}

const Animation& Assets::getAnimation(const std::string& name) const
{
    auto it = animations_.find(name);
    if (it == animations_.end())
        throwAssetNotFound("Animation", name);
    return it->second;
}

std::ifstream& Assets::getPrefabDefinition(const std::string& name)
{
    auto it = prefabs_.find(name);
    if (it == prefabs_.end())
        throwAssetNotFound("Prefab definition", name);
    return it->second;
}

bool Assets::hasTexture(const std::string& name) const
{
    return textures_.find(name) != textures_.end();
}

bool Assets::hasSound(const std::string& name) const
{
    return sounds_.find(name) != sounds_.end();
}

bool Assets::hasFont(const std::string& name) const
{
    return fonts_.find(name) != fonts_.end();
}

bool Assets::hasAnimation(const std::string& name) const
{
    return animations_.find(name) != animations_.end();
}

bool Assets::hasPrefabDefinition(const std::string& name) const
{
    return prefabs_.find(name) != prefabs_.end();
}

std::vector<std::string> Assets::getTextureNames() const
{
    std::vector<std::string> names;
    names.reserve(textures_.size());
    for (const auto& [name, _] : textures_)
        names.push_back(name);
    return names;
}

std::vector<std::string> Assets::getAnimationNames() const
{
    std::vector<std::string> names;
    names.reserve(animations_.size());
    for (const auto& [name, _] : animations_)
        names.push_back(name);
    return names;
}

std::vector<std::string> Assets::getSoundNames() const
{
    std::vector<std::string> names;
    names.reserve(sounds_.size());
    for (const auto& [name, _] : sounds_)
        names.push_back(name);
    return names;
}

std::vector<std::string> Assets::getFontNames() const
{
    std::vector<std::string> names;
    names.reserve(fonts_.size());
    for (const auto& [name, _] : fonts_)
        names.push_back(name);
    return names;
}

std::vector<std::string> Assets::getPrefabDefinitions() const
{
    std::vector<std::string> names;
    names.reserve(prefabs_.size());
    for (const auto& [name, _] : prefabs_)
        names.push_back(name);
    return names;
}