
#include "engine/editor/components_registry/ComponentLoaderRegistry.h"
#include "engine/utils/parser.h"
#include "example_game/components/CInput.h"
#include "example_game/components/CLifespan.h"
#include "example_game/components/CSpecialBullet.h"

REGISTER_COMPONENT_LOADER(CInput,
    e.addComponent<CInput>();
);

REGISTER_COMPONENT_LOADER(CLifespan, 
    float lifespanSeconds = 0.0f;

    if (args.size() >= 1) lifespanSeconds = parseFloat(args[0]);
    e.addComponent<CLifespan>(lifespanSeconds);
);

REGISTER_COMPONENT_LOADER(CSpecialBullet,
    e.addComponent<CSpecialBullet>();
);