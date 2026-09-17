#pragma once

#include "SFML/Graphics/Color.hpp"
#include "engine/utils/math/Vector2.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <cctype>

// String splitting

// Simple split by spaces (keeps your original behaviour)
inline std::vector<std::string> splitBySpace(const std::string& line)
{
    std::vector<std::string> tokens;
    std::string current;

    for (char c : line)
    {
        if (std::isspace(static_cast<unsigned char>(c)))
        {
            if (!current.empty())
            {
                tokens.push_back(current);
                current.clear();
            }
        }
        else
        {
            current += c;
        }
    }

    if (!current.empty())
        tokens.push_back(current);

    return tokens;
}

// Smarter split that treats "(x,y)" as a single token
// Use this for component lines that contain parentheses
inline std::vector<std::string> splitComponentArgs(const std::string& line)
{
    std::vector<std::string> tokens;
    std::string current;
    int parenDepth = 0;

    for (char c : line)
    {
        if (c == '(')
        {
            parenDepth++;
            current += c;
        }
        else if (c == ')')
        {
            parenDepth--;
            current += c;
        }
        else if (std::isspace(static_cast<unsigned char>(c)) && parenDepth == 0)
        {
            if (!current.empty())
            {
                tokens.push_back(current);
                current.clear();
            }
        }
        else
        {
            current += c;
        }
    }

    if (!current.empty())
        tokens.push_back(current);

    return tokens;
}

// Parsing helpers

inline Vec2f parseVec2f(const std::string& s)
{
    // Accepts: "(x,y)" or "(x, y)"
    if (s.size() < 5 || s.front() != '(' || s.back() != ')')
        throw std::runtime_error("Invalid Vec2 format: " + s);

    std::string inner = s.substr(1, s.size() - 2); // remove ( )

    size_t comma = inner.find(',');
    if (comma == std::string::npos)
        throw std::runtime_error("Missing comma in Vec2: " + s);

    try
    {
        float x = std::stof(inner.substr(0, comma));
        float y = std::stof(inner.substr(comma + 1));
        return { x, y };
    }
    catch (const std::exception&)
    {
        throw std::runtime_error("Failed to parse Vec2 numbers: " + s);
    }
}

inline Vector2<int> parseVec2(const std::string& s)
{
    // Accepts: "(x,y)" or "(x, y)"
    if (s.size() < 5 || s.front() != '(' || s.back() != ')')
        throw std::runtime_error("Invalid Vec2 format: " + s);

    std::string inner = s.substr(1, s.size() - 2); // remove ( )

    size_t comma = inner.find(',');
    if (comma == std::string::npos)
        throw std::runtime_error("Missing comma in Vec2: " + s);

    try
    {
        int x = std::stof(inner.substr(0, comma));
        int y = std::stof(inner.substr(comma + 1));
        return { x, y };
    }
    catch (const std::exception&)
    {
        throw std::runtime_error("Failed to parse Vec2 numbers: " + s);
    }
}

// Optional: parse a single float with a clear error message
inline float parseFloat(const std::string& s)
{
    try
    {
        return std::stof(s);
    }
    catch (const std::exception&)
    {
        throw std::runtime_error("Failed to parse float: " + s);
    }
}

inline sf::Color parseHexToColor(const std::string& hex) {
    std::string s = hex;
    if (!s.empty() && s[0] == '#')
        s.erase(0, 1);

    unsigned int value = std::stoul(s, nullptr, 16);

    // If input is 6 digits (RGB), pad with FF for opaque alpha
    if (s.size() == 6)
        value = (value << 8) | 0xFF;

    return sf::Color(value);
}

inline int parseInt(const std::string& s)
{
    try {
        return std::stoi(s);
    }
    catch (const std::exception&) {
        throw std::runtime_error("Failed to parse int: " + s);
    }
}

inline int8_t parseInt8(const std::string& s)
{
    int value = parseInt(s);
    if (value < -128 || value > 127)
        throw std::runtime_error("Value out of range for int8_t: " + s);
    return static_cast<int8_t>(value);
}

inline uint16_t parseUint16(const std::string& s)
{
    int value = parseInt(s);
    if (value < 0 || value > 65535)
        throw std::runtime_error("Value out of range for uint16_t: " + s);
    return static_cast<uint16_t>(value);
}