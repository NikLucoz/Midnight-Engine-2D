#include "TileMapLoader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
    bool parseBool(const std::string& value)
    {
        if (value == "true") return true;
        if (value == "false") return false;
        throw std::runtime_error("Expected true or false, got: " + value);
    }

    std::vector<std::string> readLines(const std::string& filePath)
    {
        std::ifstream file(filePath);
        if (!file.is_open())
            throw std::runtime_error("Failed to open level file: " + filePath);

        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line))
            lines.push_back(line);
        return lines;
    }
}

TileMap TileMapLoader::load(const std::string& filePath)
{

    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open asset file: " + filePath);
    }

    const std::vector<std::string> lines = readLines(filePath);
    int tileWidth = 0;
    int tileHeight = 0;
    int mapWidth = 0;
    int mapHeight = 0;
    int originX = 0;
    int originY = 0;
    bool hasTileSize = false;
    bool hasSize = false;
    int lineNumber = 0;

    std::string line = "";
    while (std::getline(file, line)) {
        ++lineNumber;
        std::istringstream stream(line);
        std::string command;
        stream >> command;

        if (command.empty() || command[0] == '#')
            continue;

        try
        {
            if (command == "TileSize")
            {
                stream >> tileWidth >> tileHeight;
                if (stream.fail()) throw std::runtime_error("invalid TileSize");
                hasTileSize = true;
            }
            else if (command == "Size")
            {
                stream >> mapWidth >> mapHeight;
                if (stream.fail()) throw std::runtime_error("invalid Size");
                hasSize = true;
            }
            else if (command == "Origin")
            {
                stream >> originX >> originY;
                if (stream.fail()) throw std::runtime_error("invalid TileSize");
            }
        }
        catch (const std::exception& error)
        {
            throw std::runtime_error(filePath + ":" + std::to_string(lineNumber) + ": " + error.what());
        }
    }

    if (!hasTileSize || !hasSize) throw std::runtime_error(filePath + ": missing TileSize or Size");

    TileMap map(tileWidth, tileHeight, mapWidth, mapHeight);
    map.setOrigin(originX, originY);
    lineNumber = 0;

    for (const std::string& line : lines)
    {
        ++lineNumber;
        std::istringstream stream(line);
        std::string command;
        stream >> command;

        if (command.empty() || command[0] == '#')
            continue;

        try
        {
            if (command == "Level" || command == "Tileset" || command == "TileSize" ||
                command == "Size" || command == "Origin")
            {
                continue;
            }
            else if (command == "Tile")
            {
                std::size_t id;
                std::string tilesetName;
                std::string animationName;
                int column;
                int row;
                std::string solidValue;
                stream >> id >> tilesetName >> column >> row >> animationName >> solidValue;
                if (stream.fail()) throw std::runtime_error("invalid Tile definition");
                if (animationName == "-") animationName.clear();

                map.addTileDefinition(Tile(
                    id, tilesetName,
                    sf::IntRect({column * tileWidth, row * tileHeight}, {tileWidth, tileHeight}),
                    animationName, parseBool(solidValue)));
            }
            else if (command == "Layer")
            {
                std::string name;
                 stream >> name;
                if (stream.fail()) throw std::runtime_error("invalid Layer definition");
                 map.addLayer(name);
            }
            else if (command == "Row")
            {
                std::string layerName;
                int row;
                stream >> layerName >> row;
                if (stream.fail()) throw std::runtime_error("invalid Row definition");

                std::vector<int> tiles;
                int tileId;
                while (stream >> tileId)
                    tiles.push_back(tileId);
                map.setRow(layerName, row, tiles);
            }
            else throw std::runtime_error("unknown command: " + command);
            
        }
        catch (const std::exception& error)
        {
            throw std::runtime_error(filePath + ":" + std::to_string(lineNumber) + ": " + error.what());
        }
    }

    return map;
}