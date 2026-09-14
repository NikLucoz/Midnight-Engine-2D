#include "GameEngine.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <algorithm>
#include <iostream>

#include "engine/actions/Action.h"
#include "engine/camera/Camera.h"
#include "engine/scenes/Scene.h"
#include "engine/utils/assets/AssetsLoader.h"
#include "engine/utils/math/Vector2.h"
#include "entities/EntityManager.h"

GameEngine::GameEngine() : GameEngine(GameConfig{}) {}

GameEngine::GameEngine(unsigned int width, unsigned int height,
                       const std::string &title)
    : GameEngine(
          GameConfig{width, height, title, Vec2f(1280.0f, 720.0f), 60, true}) {}

GameEngine::GameEngine(const GameConfig &config)
    : window_(sf::VideoMode({config.windowWidth, config.windowHeight}),
              config.windowTitle),
      bIsRunning_(false), assets_(std::make_unique<Assets>()), scenes_(),
      baseViewSize_(config.logicalViewSize), letterbox_(config.letterbox) {
  gameConfig = config;
  if (config.framerateLimit > 0)
    window_.setFramerateLimit(config.framerateLimit);
  std::cout << "Engine initialized: " << config.windowWidth << "x"
            << config.windowHeight << std::endl;
  camera_ = std::make_unique<Camera>(baseViewSize_, Vec2f(500, 500));
  handleResize(config.windowWidth, config.windowHeight);
}

void GameEngine::init() {
  AssetsLoader::loadAssetsFromFile(*assets_, "game/assets/assets.cfg");
  debugUI_.Init(window_, *assets_);
}

void GameEngine::run() {
  std::cout << "Engine running" << std::endl;
  bIsRunning_ = true;

  try {
    float accumulator = 0.0f;
    const float fixedStep =
        gameConfig.framerateLimit > 0
            ? 1.0f / static_cast<float>(gameConfig.simulationRate)
            : 1.0f / 60.0f;

    while (window_.isOpen() && bIsRunning_) {
      const float frameDelta = std::min(clock_.restart().asSeconds(), 0.25f);

      accumulator += frameDelta;
      handleEvents();

      // If lag occurs this may do more updates before rendering to catch up
      while (accumulator >= fixedStep) {
        update(fixedStep);
        accumulator -= fixedStep;
      }

      // Rendering continues whenever there is no pending simulation step
      render(frameDelta);
    }
  } catch (const std::exception &e) {
    std::cout << "Engine error: " << e.what() << std::endl;
  }

  std::cout << "Engine shutdown complete" << std::endl;
}

void GameEngine::update(float deltaTime) {
  Scene *currentScene = getCurrentScene();
  if (currentScene == nullptr) {
    throw std::runtime_error("Current scene not initialized");
  }

  EntityManager::getInstance().update();
  currentScene->update(deltaTime);
}

void GameEngine::render(float deltaTime) {
  Scene *currentScene = getCurrentScene();
  if (currentScene == nullptr) {
    throw std::runtime_error("Current scene not initialized");
  }

  window_.clear(sf::Color::Black);
  camera_->update(deltaTime);
  camera_->applyTo(window_);
  drawTestGrid(window_);
  currentScene->sRender(deltaTime);
  window_.setView(window_.getDefaultView());

  debugUI_.SetVisible(debugUI_.GetOptions().showDebugUI);
  DebugRuntimeInfo runtimeInfo;
  runtimeInfo.framesPerSecond = deltaTime > 0.0f ? 1.0f / deltaTime : 0.0f;
  runtimeInfo.windowSize = window_.getSize();
  runtimeInfo.currentScene = currentScene_;
  runtimeInfo.loadedScenes = getSceneNames();
  runtimeInfo.textures = assets_->getTextureNames();
  runtimeInfo.animations = assets_->getAnimationNames();
  runtimeInfo.sounds = assets_->getSoundNames();
  runtimeInfo.fonts = assets_->getFontNames();
  debugUI_.Update(
      sf::seconds(deltaTime), runtimeInfo,
      [currentScene]() { currentScene->sDebugUI(); },
      [this](const std::string &sceneName) { changeScene(sceneName); });
  debugUI_.Render();

  window_.display();
}

void GameEngine::quit() {
  window_.close();
  bIsRunning_ = false;
}

void GameEngine::handleEvents() {
  while (auto event = window_.pollEvent()) {
    debugUI_.ProcessEvent(*event);

    if (event->is<sf::Event::Closed>()) {
      quit();
    }

    if (const auto *resized = event->getIf<sf::Event::Resized>()) {
      handleResize(resized->size.x, resized->size.y);
    }

    if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
      if (keyPressed->code == sf::Keyboard::Key::F3 ||
          !debugUI_.WantsKeyboardInput()) {
        handleUserKeyboardInputEvent(keyPressed->code, "pressed");
      }
    }

    if (const auto *keyReleased = event->getIf<sf::Event::KeyReleased>()) {
      if (keyReleased->code != sf::Keyboard::Key::F3 &&
          !debugUI_.WantsKeyboardInput()) {
        handleUserKeyboardInputEvent(keyReleased->code, "released");
      }
    }

    if (const auto *mouseButtonPressed =
            event->getIf<sf::Event::MouseButtonPressed>()) {
      if (!debugUI_.WantsMouseInput()) {
        handleUserMouseInputEvent(mouseButtonPressed->button, "pressed",
                                  Vector2<int>(mouseButtonPressed->position.x,
                                               mouseButtonPressed->position.y));
      }
    }

    if (const auto *mouseButtonReleased =
            event->getIf<sf::Event::MouseButtonReleased>()) {
      if (!debugUI_.WantsMouseInput()) {
        handleUserMouseInputEvent(
            mouseButtonReleased->button, "released",
            Vector2<int>(mouseButtonReleased->position.x,
                         mouseButtonReleased->position.y));
      }
    }
  }
}

void GameEngine::handleUserKeyboardInputEvent(sf::Keyboard::Key keyCode,
                                              const std::string &actionType) {
  const InputBinding binding{InputDevice::Keyboard, static_cast<int>(keyCode)};

  auto &actionMap = getCurrentScene()->getActionMap();
  auto action = actionMap.find(binding);

  if (action == actionMap.end())
    return;

  getCurrentScene()->doAction(Action(action->second, actionType));
}

void GameEngine::handleUserMouseInputEvent(sf::Mouse::Button button,
                                           const std::string &actionType,
                                           const Vector2<int> pos) {
  const InputBinding binding{InputDevice::MouseButton,
                             static_cast<int>(button)};

  auto &actionMap = getCurrentScene()->getActionMap();
  auto action = actionMap.find(binding);

  if (action == actionMap.end())
    return;

  getCurrentScene()->doAction(Action(action->second, actionType, pos));
}

void GameEngine::changeScene(const std::string &sceneName) {
  if (scenes_.find(sceneName) == scenes_.end()) {
    throw std::invalid_argument("Scene '" + sceneName + "' is not registered");
  }

  if (currentScene_ == sceneName) {
    std::cout << "Scene '" << sceneName << "' is already the current scene."
              << std::endl;
    return;
  }

  if (!currentScene_.empty()) {
    getCurrentScene()->destroy();
  }

  currentScene_ = sceneName;
  getCurrentScene()->init();
}

Assets &GameEngine::getAssets() const { return *assets_; }

Camera &GameEngine::getCamera() const { return *camera_; }

sf::RenderWindow &GameEngine::getWindow() { return window_; }

const std::string &GameEngine::getCurrentSceneName() const {
  return currentScene_;
}

std::vector<std::string> GameEngine::getSceneNames() const {
  std::vector<std::string> names;
  for (const auto &[name, scene] : scenes_)
    names.push_back(name);
  return names;
}

void GameEngine::handleResize(unsigned int width, unsigned int height) {
  if (height == 0)
    return;

  if (!letterbox_) {
    camera_->setViewport(sf::FloatRect({0.0f, 0.0f}, {1.0f, 1.0f}));
    return;
  }

  const sf::Vector2f logicalViewSize = camera_->getViewSize();
  const float targetAspect = logicalViewSize.x / logicalViewSize.y;

  const float windowAspect =
      static_cast<float>(width) / static_cast<float>(height);

  sf::FloatRect viewport({0.0f, 0.0f}, {1.0f, 1.0f});

  if (windowAspect > targetAspect) {
    // Window is wider than 16:9 add left/right bars.
    viewport.size.x = targetAspect / windowAspect;
    viewport.position.x = (1.0f - viewport.size.x) * 0.5f;
  } else if (windowAspect < targetAspect) {
    // Window is taller than 16:9 add top/bottom bars.
    viewport.size.y = windowAspect / targetAspect;
    viewport.position.y = (1.0f - viewport.size.y) * 0.5f;
  }

  camera_->setViewport(viewport);
}

void GameEngine::drawTestGrid(sf::RenderWindow &window) {
  const float gridSpacing = 64.0f;
  const Vec2f cameraPosition = camera_->getPosition();
  const sf::Vector2f cameraViewSize = camera_->getViewSize();

  const float viewLeft = cameraPosition.x - cameraViewSize.x / 2.0f;
  const float viewRight = cameraPosition.x + cameraViewSize.x / 2.0f;
  const float viewTop = cameraPosition.y - cameraViewSize.y / 2.0f;
  const float viewBottom = cameraPosition.y + cameraViewSize.y / 2.0f;

  const float left = std::floor(viewLeft / gridSpacing) * gridSpacing;
  const float top = std::floor(viewTop / gridSpacing) * gridSpacing;

  const sf::Color blue(28, 27, 30);
  const sf::Color dark_blue(17, 16, 21);

  for (float y = top; y < viewBottom; y += gridSpacing) {
    for (float x = left; x < viewRight; x += gridSpacing) {
      sf::RectangleShape square({gridSpacing, gridSpacing});
      square.setPosition({x, y});

      // Checkerboard pattern
      const int col = static_cast<int>(std::floor(x / gridSpacing));
      const int row = static_cast<int>(std::floor(y / gridSpacing));

      square.setFillColor((col + row) % 2 == 0 ? blue : dark_blue);
      window.draw(square);
    }
  }
}