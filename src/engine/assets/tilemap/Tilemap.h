#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <vector>

#include <SFML/Graphics/RenderWindow.hpp>

#include "Tile.h"
#include "engine/utils/math/Vector2.h"

class Assets;

struct TileLayer
{
    std::string name;
    std::vector<int> tiles;
};

using TileDefinitions = std::map<std::size_t, Tile>;

class TileMap
{
    int tileWidth_ = 0;
    int tileHeight_ = 0;
    int width_ = 0;
    int height_ = 0;
    int originX_ = 0;
    int originY_ = 0;
    TileDefinitions tileDefinitions_;
    std::vector<TileLayer> layers_;

public:
    TileMap() = default;
    TileMap(int tileWidth, int tileHeight, int width, int height);

    void setOrigin(int x, int y);
    Vector2<int> getOrigin();
    void addTileDefinition(Tile definition);
    void addLayer(std::string name);
    void setRow(const std::string& layerName, int row, const std::vector<int>& tiles);
    void setTile(const std::string& layerName, int column, int row, int tileId);

    TileDefinitions getTileDefinitions();
    int getTileWidth() const;
    int getTileHeight() const;
    int getWidth() const;
    int getHeight() const;
    const std::vector<TileLayer>& getLayers() const;
    TileLayer& getLayer(const std::string& layerName);
    const TileLayer& getLayer(const std::string& layerName) const;

    void drawLayer(sf::RenderWindow& window, Assets& assets, TileLayer& layer) const;
};