#pragma once

#include "Entity.h"
#include "EntityController.h"
#include "Vendor.h"

namespace cute {

class ECItemPickerUpper : public EntityController {
    Q_OBJECT

public:
    ECItemPickerUpper(Entity *entity);

public slots:
    void on_collide(Entity *controlled_entity, Entity *collided_with);
};

} // namespace cute
