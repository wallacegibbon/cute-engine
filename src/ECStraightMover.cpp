#include "ECStraightMover.h"
#include "QtUtilities.h"
#include "Utilities.h"

using namespace cute;

ECStraightMover::ECStraightMover(Entity *entity) : ECMover(entity) {
    speed_ = entity->speed();
    move_timer_ = new QTimer(this);
}

void ECStraightMover::move_entity_(const QPointF &pos) {
    Entity *the_entity = entity();

    /// stop moving (just in case already moving)
    stop_moving_entity();

    target_pos_ = pos;

    /// store initial angle (so we know when the entity has past its target point)
    QLineF line(the_entity->pos(), pos);
    initial_angle_ = line.angle();

    /// face target position (if option enabled)
    if (face_target()) {
        the_entity->set_facing_angle(-1 * line.angle());
    }

    /// start moving
    connect(move_timer_, &QTimer::timeout, this, &ECStraightMover::on_move_step);
    move_timer_->start(s_to_ms(frequency(step_size_, speed_)));
}

void ECStraightMover::on_move_step() {
    Entity *the_entity = entity();

    /// if entity has died, stop moving
    if (the_entity == nullptr) {
        stop_moving_entity();
        return;
    }

    /// move
    QLineF line(the_entity->pos(), target_pos_);
    line.setLength(step_size_);
    double new_x = the_entity->x() + line.dx();
    double new_y = the_entity->y() + line.dy();
    the_entity->set_pos(QPointF(new_x, new_y));

    /// if close enough, stop moving
    const double EPSILON = 50;
    if (QtUtils::distance(the_entity->pos(), target_pos_) < EPSILON) {
        emit entity_succesfully_moved(this);
        stop_moving_entity();
        return;
    }

    /// if past (due to large step size) stop moving
    /// if we haven't passed, diff in angles should be tiny, surely under 100
    if (qAbs(line.angle() - initial_angle_) > 100) {
        stop_moving_entity();
        return;
    }
}
