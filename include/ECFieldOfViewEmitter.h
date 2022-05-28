#pragma once

#include "Entity.h"
#include "EntityController.h"
#include "Vendor.h"

class QTimer;

namespace cute {

/// An entity controller that checks the field of view of the controlled entity
/// and emits a signal whenever other entities enter or leave the controlled entity's field of view.

class ECFieldOfViewEmitter : public EntityController {
    Q_OBJECT
public:
    ECFieldOfViewEmitter(Entity *entity, double FOV_angle = 90, double FOV_distance = 600);
    ~ECFieldOfViewEmitter();

    std::unordered_set<Entity *> entities_in_view();

    void set_check_frequency(double times_per_second);
    double check_frequency() const;

    void turn_on();
    void turn_off();

    void set_show_FOV(bool tf);

public slots:
    void check_FOV();

signals:
    void entity_entered_FOV(Entity *entity);
    void entity_left_FOV(Entity *entity);

private:
    void ensure_visual_FOV_removed();

private:
    double field_of_view_angle_;
    double field_of_view_distance_;
    double field_of_view_check_delay_ms_ = 50;
    QTimer *timer_check_FOV_;

    bool show_FOV_ = false;
    QGraphicsPolygonItem *visual_FOV_;

    std::unordered_set<Entity *> entities_in_view_last_time_;
};

} // namespace cute
