#pragma once

#include "Entity.h"
#include "EntityController.h"
#include "Vendor.h"

class QPointF;

namespace cute {

class ECMapMover : public EntityController {
    Q_OBJECT

public:
    ECMapMover(Entity *entity);
    void set_border_threshold(double threshold) { border_threshold_ = threshold; }
    double border_threshold() { return border_threshold_; }

public slots:
    void on_entity_moved(Entity *controlled_entity, QPointF from_pos, QPointF to_pos);

private:
    /* the border_threshold_ should be bigger than ECKeyboardMoverPerspective::step_size_ */
    double border_threshold_ = 20;
};

} // namespace cute
