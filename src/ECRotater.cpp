#include "ECRotater.h"
#include "Entity.h"
#include "Sprite.h"
#include "Utilities.h"

using namespace cute;

ECRotater::ECRotater(Entity *entity) : EntityController(entity) {
    rotation_timer_ = new QTimer(this);
    connect(rotation_timer_, &QTimer::timeout, this, &ECRotater::on_rotate_step);
}

/// Rotate the entity until it faces the specified angle. Angle must be between 0-360 inclusive.
/// A specified angle of 0 degrees is right, 90 degrees is down and so on (in other words, angle increases clockwise).
void ECRotater::rotate_towards(int angle) {
    assert(angle >= 0 && angle <= 360);

    QLineF line(QPointF(0, 0), QPointF(1, 1));
    line.setAngle(-entity_controlled()->facing_angle());

    QLineF line2(QPointF(0, 0), QPointF(1, 1));
    line2.setAngle(-angle);

    double angle_between_lines = line.angleTo(line2);

    if (angle_between_lines < 180) {
        rotate_left(angle_between_lines);
    } else {
        rotate_right(360 - angle_between_lines);
    }
}

void ECRotater::rotate_towards(const QPointF &point) {
    QLineF line(entity_controlled()->pos(), point);
    int r = 360 - line.angle();
    rotate_towards(r);
}

void ECRotater::rotate_left(int degrees) {
    /// stop previous rotations
    stop_rotating();

    Entity *entity = entity_controlled();
    target_angle_ = entity->facing_angle() - degrees;
    rotate_right_ = false;
    rotation_timer_->start(s_to_ms(frequency(step_size_, entity->rotation_speed())));
}

void ECRotater::rotate_right(int degrees) {
    /// stop previous rotations
    stop_rotating();

    Entity *entity = entity_controlled();
    target_angle_ = entity->facing_angle() + degrees;
    rotate_right_ = true;
    rotation_timer_->start(s_to_ms(frequency(step_size_, entity->rotation_speed())));
}

void ECRotater::on_rotate_step() {
    /// if the entity has been destroyed, do nothing
    Entity *entity = entity_controlled();
    if (entity == nullptr) {
        stop_rotating();
        return;
    }

    /// if it has reached its target_angle, stop rotating
    /// other wise, rotate once towards target_angle
    if (abs(entity->facing_angle() - target_angle_) == 0) {
        rotation_timer_->stop();
    } else {
        rotate_towards_target_angle();
    }
}

void ECRotater::rotate_towards_target_angle() {
    /// rotate right if rotateRight other wise rotate left
    Entity *entity = entity_controlled();
    if (rotate_right_) {
        entity->set_facing_angle(entity->facing_angle() + 1);
    } else {
        entity->set_facing_angle(entity->facing_angle() - 1);
    }
}
