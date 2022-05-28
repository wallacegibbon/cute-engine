#pragma once

#include "Entity.h"
#include "EntityController.h"
#include "Vendor.h"

namespace cute {

class Map;

/// An entity controller that will set the game's current map to be the map that the controlled entity is in.

class ECCurrentMapGrabber : public EntityController {
    Q_OBJECT

public:
    ECCurrentMapGrabber(Entity *entity);

public slots:
    void on_entity_enters_map(Entity *controlled_entity, Map *map_just_entered, Map *old_map);
};

} // namespace cute
