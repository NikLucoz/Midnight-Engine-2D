#pragma once

#include "engine/utils/math/Vector2.h"
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <string>

struct GameConfig {
  unsigned int windowWidth = 1280;
  unsigned int windowHeight = 720;
  std::string windowTitle = "Game Engine";

  Vec2f logicalViewSize = Vec2f(1280.0f, 720.0f);

  unsigned int framerateLimit = 60;
  unsigned int simulationRate = 60;
  bool letterbox = true;
};

class GameConfigLoader {
public:
  static GameConfig load(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
    throw std::runtime_error("Failed to open game config file: " + filePath);
    }

  GameConfig config;

  std::string line;
  std::size_t lineNumber = 0;
    while (std::getline(file, line)) {
    ++lineNumber;

    const std::size_t commentPosition = line.find("//");
    if (commentPosition != std::string::npos) {
      line.erase(commentPosition);
    }

        std::istringstream iss(line);
    std::string key;
    if (!(iss >> key)) continue;

    auto fail = [&]() {
      throw std::runtime_error(
        filePath + ":" + std::to_string(lineNumber) +
        ": invalid value for " + key);
    };

    if (key == "windowWidth") {
      if (!(iss >> config.windowWidth)) fail();
    } else if (key == "windowHeight") {
      if (!(iss >> config.windowHeight)) fail();
    } else if (key == "windowTitle") {
      std::getline(iss >> std::ws, config.windowTitle);
      if (config.windowTitle.empty()) fail();
    } else if (key == "logicalViewWidth") {
      if (!(iss >> config.logicalViewSize.x)) fail();
    } else if (key == "logicalViewHeight") {
      if (!(iss >> config.logicalViewSize.y)) fail();
    } else if (key == "aspectPolicy") {
      std::string policy;
      if (!(iss >> policy)) {
        fail();
      } else if (policy == "letterbox") {
        config.letterbox = true;
      } else if (policy == "stretch") {
        config.letterbox = false;
      } else {
        fail();
      }
    } else if (key == "framerateLimit") {
      if (!(iss >> config.framerateLimit)) fail();
    }
    else if(key == "simulationRate") {
      if(!(iss >> config.simulationRate)) fail();
    } else {
      throw std::runtime_error(
        filePath + ":" + std::to_string(lineNumber) +
        ": unknown setting " + key);
    }
  }

  if (config.windowWidth == 0 || config.windowHeight == 0 ||
    config.logicalViewSize.x <= 0.0f || config.logicalViewSize.y <= 0.0f) {
    throw std::runtime_error(filePath + ": window and view sizes must be positive");
    }

  return config;
  }
};