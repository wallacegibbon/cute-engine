#pragma once

#include "ECMover.h"
#include "Entity.h"
#include "Vendor.h"

class QTimer;

namespace cute {

class ECSineMover : public ECMover {
    Q_OBJECT

public:
    ECSineMover(Entity *entity = nullptr);

    void set_face_target(bool tf) { face_target_ = tf; }
    bool face_target() const { return face_target_; }

    void set_speed(int speed) { speed_ = speed; }
    int speed() const { return speed_; }

    void set_step_size(int step_size) { step_size_ = step_size; }
    int step_size() const { return step_size_; }

public slots:
    void on_move_step();

protected:
    void move_entity_(const QPointF &pos) override;
    void stop_moving_entity_() override;

private:
    QTimer *move_timer_;

    double amplitude_ = 20;
    double wave_length_ = 100;
    QPointF target_pos_;
    QPointF start_pos_;

    int step_size_ = 15;
    int speed_ = 500;

    double range_ = 0;
    bool face_target_ = true;

    double distance_moved_ = 0;
};

} // namespace cute
