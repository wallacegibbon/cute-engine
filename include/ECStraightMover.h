#pragma once

#include "ECMover.h"
#include "Entity.h"
#include "Vendor.h"

class QTimer;

namespace cute {

class ECStraightMover : public ECMover {
    Q_OBJECT

public:
    ECStraightMover(Entity *entity = nullptr);

    void set_speed(int speed) { speed_ = speed; }
    int speed() { return speed_; }

    void set_step_size(int step_size) { step_size_ = step_size; }
    int step_size() { return step_size_; }

    void set_face_target(bool tf) { face_target_ = tf; }
    bool face_target() { return face_target_; }

public slots:
    void on_move_step();

protected:
    void move_entity_(const QPointF &pos) override;
    void stop_moving_entity_() override { move_timer_->disconnect(); }

private:
    int speed_;
    bool face_target_ = true;

    QTimer *move_timer_;

    double initial_angle_;
    QPointF target_pos_;
    int step_size_ = 25;
};

} // namespace cute
