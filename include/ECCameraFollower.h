#pragma once

#include "Entity.h"
#include "EntityController.h"
#include "Vendor.h"

namespace cute {

class ECCameraFollower : public EntityController {
    Q_OBJECT

public:
    ECCameraFollower(Entity *entity);

public slots:
    void on_entity_moved(Entity *entity, QPointF from_pos, QPointF to_pos);
};

} // namespace cute
