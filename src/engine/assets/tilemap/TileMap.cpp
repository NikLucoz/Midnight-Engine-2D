#include "TileMap.h"

#include <algorithm>
#include <stdexcept>

#include <SFML/Graphics/Sprite.hpp>

#include "engine/assets/Assets.h"
#include "engine/assets/tilemap/Tilemap.h"
#include "engine/utils/math/Vector2.h"

TileMap::TileMap(int tileWidth, int tileHeight, int width, int height) : tileWidth_(tileWidth), tileHeight_(tileHeight), width_(width), height_(height)
{
    if (tileWidth <= 0 || tileHeight <= 0 || width <= 0 || height <= 0)
        throw std::invalid_argument("TileMap dimensions must be positive");
}

TileLayer& TileMap::getLayer(const std::string& layerName)
{
    auto it = std::find_if(layers_.begin(), layers_.end(), [&](const TileLayer& layer) {
        return layer.name == layerName;
    });

    if (it == layers_.end())
        throw std::runtime_error("Tile layer not found: " + layerName);

    return *it;
}

const TileLayer& TileMap::getLayer(const std::string& layerName) const
{
    auto it = std::find_if(layers_.begin(), layers_.end(), [&](const TileLayer& layer) {
        return layer.name == layerName;
    });

    if (it == layers_.end())
        throw std::runtime_error("Tile layer not found: " + layerName);

    return *it;
}

void TileMap::setOrigin(int x, int y)
{
    originX_ = x;
    originY_ = y;
}

void TileMap::addTileDefinition(Tile tile)
{
    tileDefinitions_.insert_or_assign(tile.getId(), std::move(tile));
}

void TileMap::addLayer(std::string name)
{
    if (std::any_of(layers_.begin(), layers_.end(), [&](const TileLayer& layer) {
        return layer.name == name;
    }))
        throw std::runtime_error("Duplicate tile layer: " + name);

    TileLayer layer;
    layer.name = std::move(name);
    //layer.drawOrder = drawOrder;
    layer.tiles.assign(static_cast<std::size_t>(width_ * height_), -1);
    layers_.push_back(std::move(layer));

    //std::sort(layers_.begin(), layers_.end(), [](const TileLayer& left, const TileLayer& right) {
    //    return left.drawOrder < right.drawOrder;
    //});
}

void TileMap::setRow(const std::string& layerName, int row, const std::vector<int>& tiles)
{
    if (row < 0 || row >= height_)
        throw std::out_of_range("Tile row is outside the map");
    if (static_cast<int>(tiles.size()) != width_)
        throw std::runtime_error("Tile row has an invalid width in layer: " + layerName);

    TileLayer& layer = getLayer(layerName);
    std::copy(tiles.begin(), tiles.end(), layer.tiles.begin() + row * width_);
}

void TileMap::setTile(const std::string& layerName, int column, int row, int tileId)
{
    if (column < 0 || column >= width_ || row < 0 || row >= height_)
        throw std::out_of_range("Tile position is outside the map");

    TileLayer& layer = getLayer(layerName);
    layer.tiles[static_cast<std::size_t>(row * width_ + column)] = tileId;
}


Vector2<int> TileMap::getOrigin() { return {originX_, originY_}; }
int TileMap::getTileWidth() const { return tileWidth_; }
int TileMap::getTileHeight() const { return tileHeight_; }
int TileMap::getWidth() const { return width_; }
int TileMap::getHeight() const { return height_; }
TileDefinitions TileMap::getTileDefinitions() { return tileDefinitions_; }
const std::vector<TileLayer>& TileMap::getLayers() const { return layers_; }



void TileMap::drawLayer(sf::RenderWindow& window, Assets& assets, TileLayer& layer) const
{
    for (int row = 0; row < height_; ++row)
    {
        for (int column = 0; column < width_; ++column)
        {
            const int tileId = layer.tiles[static_cast<std::size_t>(row * width_ + column)];
            if (tileId < 0)
                continue;

            auto definitionIt = tileDefinitions_.find(static_cast<std::size_t>(tileId));
            if (definitionIt == tileDefinitions_.end())
                throw std::runtime_error("Tile definition not found: " + std::to_string(tileId));

            const Tile& definition = definitionIt->second;
            sf::Sprite sprite(assets.getTexture(definition.getTilesetName()));
            sprite.setTextureRect(definition.getTextureRect());
            sprite.setPosition(sf::Vector2f(
                static_cast<float>(originX_ + column * tileWidth_),
                static_cast<float>(window.getSize().y - originY_ - (row + 1) * tileHeight_)));
            window.draw(sprite);
        }
    }
}