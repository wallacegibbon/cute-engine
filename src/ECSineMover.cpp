#include "ECSineMover.h"
#include "Utilities.h"

using namespace cute;

ECSineMover::ECSineMover(Entity *entity) : ECMover(entity), target_pos_(), start_pos_() {
    move_timer_ = new QTimer(this);
}

void ECSineMover::move_entity_(const QPointF &pos) {
    Entity *the_entity = entity();

    assert(the_entity != nullptr);

    /// stop moving (just in case already moving)
    stop_moving_entity();

    /// update internal variables
    target_pos_ = pos;
    start_pos_ = the_entity->pos();

    /// calculate range (so we know when we have gone past the target)
    QLineF line(the_entity->pos(), pos);
    range_ = line.length();

    /// face target position (if option enabled)
    if (face_target_) {
        the_entity->set_facing_angle(-1 * line.angle());
    }

    /// start moving
    connect(move_timer_, &QTimer::timeout, this, &ECSineMover::on_move_step);
    move_timer_->start(s_to_ms(frequency(step_size_, speed_)));
}

void ECSineMover::on_move_step() {
    Entity *the_entity = entity();

    /// if entity has died, stop moving
    if (the_entity == nullptr) {
        stop_moving_entity();
        return;
    }

    distance_moved_ += step_size_;

    QLineF start_to_target_line(start_pos_, target_pos_);

    /// calculate horizontal component
    QLineF h_component(start_to_target_line);
    h_component.setLength(distance_moved_);

    double PI = 3.14159;
    double y_offset = amplitude_ * qSin(2 * PI * distance_moved_ / wave_length_);

    QLineF v_component(h_component.p2(), QPointF(0, 0));
    v_component.setAngle(h_component.angle());
    v_component.setLength(qAbs(y_offset));

    if (y_offset > 0) {
        v_component.setAngle(v_component.angle() + 90);
    } else {
        v_component.setAngle(v_component.angle() - 90);
    }

    /// move to final location
    the_entity->set_pos(v_component.p2());

    /// if moved far enough
    if (distance_moved_ > range_) {
        emit entity_succesfully_moved(this);
        stop_moving_entity();
    }
}

void ECSineMover::stop_moving_entity_() {
    move_timer_->disconnect();
    distance_moved_ = 0;
}
