#pragma once

#include "Entity.h"
#include "EntityController.h"
#include "Vendor.h"

class QTimer;

namespace cute {

class ECRotater : public EntityController {
    Q_OBJECT

public:
    ECRotater(Entity *entity);

    void rotate_towards(int angle);
    void rotate_towards(const QPointF &point);
    void rotate_left(int degrees);
    void rotate_right(int degrees);
    void stop_rotating() { rotation_timer_->stop(); }

    /// This function only returns true if the *ECRotater* is the one rotating the Entity,
    /// not if the Entity is being rotated due to itself or some other object.
    bool is_rotating() const { return rotation_timer_->isActive(); }

    void set_step_size(double degrees) { step_size_ = degrees; }
    double step_size() { return step_size_; }

public slots:
    void on_rotate_step();

private:
    void rotate_towards_target_angle();

private:
    double step_size_ = 1;

    QTimer *rotation_timer_;

    bool rotate_right_ = false;
    int target_angle_ = 0;
};

} // namespace cute
