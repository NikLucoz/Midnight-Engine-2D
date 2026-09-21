# What is Midnight Engine?

<table>
<tr>
<td width="200">

![Engine logo](src/engine_logo.png)

</td>
<td>

Midnight Engine is a simple 2D Game Engine written in C++ that uses ECS Architecture (Entity Component System). It was made to better learn C++ and understand how game engines work internally. <br> 
Part of the architecture resembles what is shown in the lessons of <a href="https://davechurchill.ca/" target="_blank">Professor Dave Churchill</a> of <a href="https://www.mun.ca/" target="_blank">Memorial University of Newfoundland</a>. <br> 
I followed his lessons to better understand the fundamentals behind an engine and then expanded its features from there. <br> 
The engine's name comes from the hours I usually worked on it. Since I often worked late at night, Midnight was the perfect fit!

</td>
</tr>
</table>

## Table of Contents
1. [What is Midnight Engine?](#what-is-midnight-engine)
2. [How do entities and components work?](#how-do-entities-and-components-work)
3. [How do I create a new component?](#how-do-i-create-a-new-component)
4. [How do I register a component so the engine can load it from a prefab file?](#how-do-i-register-a-component-so-the-engine-can-load-it-from-a-prefab-file)
5. [How do I register a component so it shows up in the editor inspector?](#how-do-i-register-a-component-so-it-shows-up-in-the-editor-inspector)
6. [How do actions and input work?](#how-do-actions-and-input-work)
   - [Registering bindings](#registering-bindings)
   - [Handling actions](#handling-actions)
   - [Using input in game logic](#using-input-in-game-logic)
7. [How do I put this all together in a new scene?](#how-do-i-put-this-all-together-in-a-new-scene)
8. [How do I add custom assets?](#how-do-i-add-custom-assets)
   - [Texture](#texture)
   - [Font](#font)
   - [Sound](#sound)
   - [Animation](#animation)
   - [Prefab](#prefab)
9. [How do prefab files work?](#how-do-prefab-files-work)
   - [Keywords](#keywords)
   - [A minimal prefab](#a-minimal-prefab)
   - [Adding a child via external prefab reference](#adding-a-child-via-external-prefab-reference)
   - [Adding a child inline](#adding-a-child-inline)
   - [Spawning a prefab in code](#spawning-a-prefab-in-code)
10. [How do rendering layers work?](#how-do-rendering-layers-work)
    - [Render layers and ordering](#render-layers-and-ordering)
    - [Parent-child positioning](#parent-child-positioning)
11. [Planned new features](#planned-new-features)
12. [Info on requirements and building the engine](#info-on-requirements-and-building-the-engine)

## How do entities and components work?

An `Entity` represents any object in a scene. Conceptually it aggregates different components, although the components themselves are stored and managed by a `ComponentManager`. Each entity can hold only one component of a given type. If you need multiple components of the same type, you must create child entities.

You interact with components through these template methods on `Entity`:

```cpp
entity->addComponent<CTransform>(position, velocity, rotation, scale);
entity->hasComponent<CTransform>();     // returns bool
entity->getComponent<CTransform>();     // returns T&
entity->removeComponent<CTransform>();
```

There is also a shorthand for adding renderable components:

```cpp
entity->addRenderable<CSprite>(size, origin, color);
// equivalent to addComponent<CRenderable>() + addComponent<CSprite>(...)
```

Entities are **only** created through `EntityManager`. You never construct one directly:

```cpp
auto e = EntityManager::getInstance().addEntity("enemy", "goblin");
e->addComponent<CTransform>(Vec2f{100, 200});
e->addComponent<CShape>(16.f, 6, sf::Color::Red, sf::Color::White, 2.f);
```

Adds are **deferred**, the entity enters the live lists only after `EntityManager::update()` is called (once per frame by the engine). Destroys are also deferred the same way. To destroy an entity and all its children:

```cpp
entity->destroy();
```

Querying entities:

```cpp
EntityManager::getInstance().getEntities();             // all
EntityManager::getInstance().getEntities("enemy");      // by tag
EntityManager::getInstance().getEntitiesInScene("play");
EntityManager::getInstance().getEntityWithId(id);
```

You manage the entity hierarchy through these methods on Entity:
```cpp
C++entity->addChild(child);     // adds child
entity->removeChild(child);     // removes child and clears its parent
entity->hasChild(child);        // returns bool
entity->hasChild(id);           // same, by id

entity->getChildrens();         // returns const std::vector<size_t>&
entity->getParent();            // returns parent id (-1 if none)
entity->setParent(id);          // sets parent

entity->changeChildOrder(id, index); // moves a child to a new position in the list
```

addChild automatically removes the child from its previous parent (if any). Adding an entity as a child of one of its own descendants is ignored to avoid cycles.

---

## How do I create a new component?

A component is just a plain data struct that inherits from `Component`. Put it wherever makes sense, typically in `src/game_name/components/`.

```cpp
// src/game_name/components/CHealth.h
#pragma once
#include "engine/components/Component.h"

struct CHealth : Component {
    int current;
    int max;

    CHealth(int max) : current(max), max(max) {}
};
```

That is the entire definition. No registration, no ID, no macro needed for the component itself.

---

## How do I register a component so the engine can load it from a prefab file?

Prefab files drive entity creation. The loader reads lines like:

```
COMPONENT CHealth 100
```

and calls the matching loader function. You register that function with the `REGISTER_COMPONENT_LOADER` macro. Add a `.cpp` file (or add to an existing one like `GameComponentLoaders.cpp`):

```cpp
// src/game_name/engine/GameComponentLoaders.cpp
#include "engine/editor/components_registry/ComponentLoaderRegistry.h"
#include "game_name/components/CHealth.h"
#include "engine/utils/parser.h"

REGISTER_COMPONENT_LOADER(CHealth,
    int maxHp = 100;
    if (args.size() >= 1) maxHp = parseInt(args[0]);
    e.addComponent<CHealth>(maxHp);
);
```

The macro receives `Entity& e` and `const std::vector<std::string>& args` automatically. The args are the space-separated tokens after the component name in the prefab file. The registration happens at static init time, so just having this `.cpp` compiled is enough.

---

## How do I register a component so it shows up in the editor inspector?

Add a `REGISTER_COMPONENT_DRAWER` entry, again typically in `GameComponentDrawers.cpp`:

```cpp
// src/game_name/engine/GameComponentDrawers.cpp
#include "engine/editor/components_registry/ComponentDrawerRegistry.h"
#include "game_name/components/CHealth.h"

REGISTER_COMPONENT_DRAWER(CHealth, "CHealth", {
    if (ImGui::TreeNode("CHealth")) {
        ImGui::Text("HP: %d / %d", c.current, c.max);
        ImGui::SliderInt("Current", &c.current, 0, c.max);
        ImGui::TreePop();
    }
});
```

The macro receives a reference `c` of the correct type automatically, no casting needed on your side. The entity inspector calls every registered drawer for each component the selected entity actually has.

---

## How do actions and input work?

An `Action` is a named event with a type (`"pressed"` or `"released"`). Instead of reading raw key states in your game logic, the engine translates hardware input into actions and your scene reacts to those names.

### Registering bindings

In your scene's constructor, call `registerAction` for every input you care about:

```cpp
// inside ScenePlay constructor
registerAction(InputDevice::Keyboard,    sf::Keyboard::Key::W,          "Move_Up");
registerAction(InputDevice::Keyboard,    sf::Keyboard::Key::S,          "Move_Down");
registerAction(InputDevice::Keyboard,    sf::Keyboard::Key::A,          "Move_Left");
registerAction(InputDevice::Keyboard,    sf::Keyboard::Key::D,          "Move_Right");
registerAction(InputDevice::MouseButton, sf::Mouse::Button::Left,       "Shoot");
registerAction(InputDevice::MouseButton, sf::Mouse::Button::Right,      "Special");
```

The base `Scene` class already registers `F3 → "Toggle_Debug_UI"` for you.

### Handling actions

Implement `sDoAction` in your scene. It is called once per input event:

```cpp
void ScenePlay::sDoAction(const Action& action) {
    if (action.name() == "Move_Up") {
        player_->getComponent<CInput>().bUp = (action.type() == "pressed");
    }
    if (action.name() == "Shoot" && action.type() == "pressed") {
        spawnBullet(player_);
    }
}
```

### Using input in game logic

For movement the typical pattern is to store the state in a `CInput` component and read it in your movement system each frame:

```cpp
// inside sMovement(float dt)
auto& input = player_->getComponent<CInput>();
auto& tf    = player_->getComponent<CTransform>();

tf.velocity = input.getMovementDirection() * speed;
tf.position += tf.velocity * dt;
```

`CInput` is just a struct of booleans, `bUp`, `bDown`, `bLeft`, `bRight`, `bMouseLeft`, `bMouseRight`, that get flipped by `sDoAction` and consumed by the movement system. You can add whatever fields your game needs.

---

## How do I put this all together in a new scene?

1. Subclass `Scene` and implement the pure virtuals: `init`, `destroy`, `update`, `sRender`, `sDebug`, `sDoAction`.
2. In the constructor, call `registerAction` for every input binding.
3. In `init`, spawn your entities via `EntityManager` and attach components.
4. In `sDoAction`, translate action names to `CInput` or trigger immediate effects.
5. In `update`, run your systems (movement, collision, lifespan, etc.) in the order you want.

That is the whole loop. The engine handles the event pump, frame timing, and editor UI. Your scene only needs to know about actions and entities.

---

## How do I add custom assets?

All assets are declared in a single config file: `game/assets/assets.cfg`. The engine loads it automatically at startup via `AssetsLoader::loadAssetsFromFile`. Each line follows a simple `<Type> <name> <args...>` format. Lines starting with `//` or `#` are ignored.

### Texture

```
Texture <name> <path>

Texture playerSheet  game/assets/sprites/player.png
Texture mapTileset   game/assets/sprites/tileset.png
```

The name is what you use everywhere else in the engine to refer to this texture.

In code, after loading:

```cpp
sf::Texture& tex = Assets::getInstance().getTexture("playerSheet");
```

### Font

```
Font <name> <path>

Font fontArial  game/assets/fonts/arial.ttf
```

In code:

```cpp
sf::Font& font = Assets::getInstance().getFont("fontArial");
```

### Sound

```
Sound <name> <path>

Sound jumpSfx  game/assets/sfx/jump.ogg
Sound boomSfx  game/assets/sfx/boom.ogg
```

In code:

```cpp
sf::Sound& sound = Assets::getInstance().getSound("jumpSfx");
sound.play();
```

### Animation

Animations reference a texture that must already be declared above them in the file. The texture is expected to be a horizontal sprite sheet.

```
Animation <name> <textureName> <frameCount> <speed> <loopable> [frameWidth] [frameHeight]

Animation gumbaWalk  gumbaWalking  2  10  true  16  16
```

- `frameCount` - number of frames in the sheet
- `speed` - frames per second
- `loopable` - `true` or `false`; non-loopable animations clamp on the last frame and report `hasEnded()`
- `frameWidth` / `frameHeight` - optional; if omitted the engine assumes square frames equal to the texture height

In code:

```cpp
Animation& anim = Assets::getInstance().getAnimation("gumbaWalk");
```

To attach it to an entity use `CAnimatedSprite`.

### Prefab

```
Prefab <name> <path>

Prefab PlayerPrefab  game/prefabs/player_prefab.cfg
Prefab GoblinPrefab  game/prefabs/goblin.cfg
```

The name is how you spawn the prefab at runtime:

```cpp
auto e = PrefabEntityLoader::LoadEntity("PlayerPrefab");
```

The entity is added to `EntityManager` and its shared pointer is returned. See the section below for the prefab file format.

---

## How do prefab files work?

A prefab is a plain text file that describes one entity and optionally its children. The loader reads it line by line. Lines starting with `//` or `#` are comments.

### Keywords

| Keyword            | Syntax                                | Meaning                                                       |
| ------------------ | ------------------------------------- | ------------------------------------------------------------- |
| `NAME`             | `NAME <entityName>`                   | Sets the entity name                                          |
| `TAG`              | `TAG <tag>`                           | Sets the entity tag used for `getEntities("tag")`             |
| `COMPONENT`        | `COMPONENT <TypeName> [args...]`      | Attaches a component; delegates to the loader registry        |
| `CHILD PREFAB`     | `CHILD PREFAB <prefabName>`           | Attaches a child entity loaded from another registered prefab |
| `CHILD ENTITY_DEF` | `CHILD ENTITY_DEF` … `END_ENTITY_DEF` | Attaches a child entity defined inline in the same file       |
| `END_ENTITY_DEF`   | `END_ENTITY_DEF`                      | Closes an inline child block                                  |

Arguments after `COMPONENT` are space-separated. Parenthesised groups like `(300.0,200.0)` are kept as a single token, which is how `Vec2f` values are passed.

### A minimal prefab

```
// goblin.cfg
NAME goblin
TAG enemy

COMPONENT CTransform (0.0,0.0) (0.0,0.0) 0 (1.0,1.0)
COMPONENT CShape 16 6 #00aa00 #ffffff 2
COMPONENT CBoundingBox (32,32)
```

### Adding a child via external prefab reference

Use `CHILD PREFAB` and give the **asset name** (the name you registered in `assets.cfg`, not the file path):

```
// boss.cfg
NAME boss
TAG enemy

COMPONENT CTransform (0.0,0.0) (0.0,0.0) 0 (1.0,1.0)
COMPONENT CShape 48 8 #aa0000 #ffffff 4
COMPONENT CBoundingBox (96,96)

CHILD PREFAB GoblinPrefab
```

When `boss.cfg` is loaded, the engine recursively calls `LoadEntity("GoblinPrefab")`, creates that entity, and attaches it as a child. The child inherits its own components as defined in `goblin.cfg`.

### Adding a child inline

Use `CHILD ENTITY_DEF` … `END_ENTITY_DEF` to define a child directly inside the same file without creating a separate prefab:

```
// player_prefab.cfg
NAME playerEntity
TAG player

COMPONENT CTransform (300.0,300.0) (0.0,0.0) 0 (1.0,1.0)
COMPONENT CShape 30 12 #000000 #ffffff 5
COMPONENT CInput
COMPONENT CBoundingBox (60,60)

CHILD ENTITY_DEF
NAME shield
TAG player_part
COMPONENT CTransform (50.0,0.0) (0.0,0.0) 0 (1.0,1.0)
COMPONENT CShape 15 6 #0000ff #ffffff 2
COMPONENT CBoundingBox (30,30)
END_ENTITY_DEF
```

The two methods can be mixed freely. A single prefab can have multiple `CHILD PREFAB` and `CHILD ENTITY_DEF` blocks, and they can nest arbitrarily deep.

### Spawning a prefab in code

Make sure the prefab is declared in `assets.cfg` first, then call:

```cpp
auto entity = PrefabEntityLoader::LoadEntity("PlayerPrefab");
```

The entity (and all its children) are queued in `EntityManager` and become live after the next `EntityManager::update()` call at the start of the next frame.

## How do rendering layers work?

The rendering system uses two properties in CRenderable to control draw order: renderLayer and orderInLayer. This allows you to organize what gets drawn first, second, etc., and control depth within each layer.

### Render layers and ordering

In the default rendering system, root entities (entities without parents) are first grouped by their renderLayer value. Then, within each layer, entities are sorted by their orderInLayer value before rendering. This means:

Lower renderLayer values are drawn first (background)
Higher renderLayer values are drawn last (foreground)
Within the same layer, lower orderInLayer values are drawn before higher ones

Children of root entities are rendered after their parent is rendered, regardless of their individual layer values. This ensures parent-child hierarchies stay visually coherent.

Here is the CRenderable component definition:

```cpp
#pragma once

#include "engine/components/Component.h"
#include "engine/utils/math/Vector2.h"

struct CRenderable : public Component {

    Vec2f localScale_ = {1.0f, 1.0f};
    bool visible_ = true;
    int renderLayer_ = 0;
    int orderInLayer = 0;

public:
    CRenderable(const Vec2f& localScale = {1.0f, 1.0f}, int renderLayer = 0, int orderInLayer = 0)
        : localScale_(localScale), renderLayer_(renderLayer), orderInLayer(orderInLayer) {}

    const Vec2f& getLocalScale() const { return localScale_; }
    void setLocalScale(const Vec2f& scale) { localScale_ = scale; }

    void setVisible(bool visible) { visible_ = visible; }
    bool isVisible() const { return visible_; }

    int getRenderLayer() const { return renderLayer_; }
    void setRenderLayer(int renderLayer) { this->renderLayer_ = renderLayer; }
    int getOrderInLayer() const { return orderInLayer; }
    void setOrderInLayer(int orderInLayer) { this->orderInLayer = orderInLayer; }
};
```

You can customize the rendering system to sort however you like, the default is just a convenient starting point. You can find the default one in `src/engine/rendering/RenderingSystem.h`.
A custom rendering system can be passed as a `std::unique_ptr<IRenderingSystem>` when constructing a new scene or can be left `nullptr` to use the default one.

### Parent-child positioning

When an entity has children, each child's CTransform is used as an offset from the parent's CTransform. The final world position is the parent's position plus the child's position.

For example, if a parent is positioned at (50, 50) and a child has a position value of (4, 4), the child will be drawn at world position (54, 54).

This makes it easy to build complex visuals: attach a shield sprite as a child of the player, attach decorative elements as children of a boss, etc. The parent-child relationship is maintained automatically during rendering.

## Planned new features

I'm planning to keep working on this engine to add some useful additions and hopefully make a game using it. These are some things I will work on in the near future:

- Refactoring scene logic into reusable systems
- Adding sprite flipping and origin configuration
- Adding Y-sorting
- Adding Tile painting from engine view (debug mode with F3)
- Adding Trigger collider components
- Adding generated collision geometry for tilemaps
- Better editor view and tools (Entity creator, Scene creator etc)


## Info on requirements and building the engine

### Requirements

- CMake 3.20 or higher
- C++ compiler with C++17 support
- JetBrains Rider

### Building and Running

This project uses CMake Presets for configuration. Open the project in Rider and use the IDE's built-in CMake tools to build and run:

1. Open the project in Rider
2. Rider will automatically detect the CMake configuration from `CMakePresets.json`
3. Select the desired build configuration from the toolbar
4. Use the Run/Debug buttons to build and execute the project

### Project Structure

- `src/engine` - Engine Source files
- `src/example_game` - Example game for engine testing purposes
- `CMakeLists.txt` - CMake configuration
- `CMakePresets.json` - CMake presets for IDE integration
