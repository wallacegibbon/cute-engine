#pragma once

#include "MeleeWeapon.h"
#include "Vendor.h"

namespace cute {

class Sound;

class Spear : public MeleeWeapon {
    Q_OBJECT

public:
    Spear();

    void attack(QPointF position) override;

    double thrust_distance() { return thrust_distance_; }
    double thrust_speed() { return thrust_speed_; }
    void set_thrust_speed(double speed);
    void set_thrust_distance(double distance);

public slots:
    void thrust_step();

private:
    void reset_variables();

private:
    QTimer *timer_;
    int thrust_step_frequency_;
    int max_thrust_steps_;
    int current_thrust_steps_ = 0;
    double thrust_length_each_step_ = 5;

    /// the number of pixels the spear thrusts.
    /// calculate from other attributes, here for perf
    double thrust_distance_;

    /// the number of pixels per second the spear thrusts at.
    /// calculated from other attributes, here for perf
    double thrust_speed_;

    /// is the spear heading forward or backward?
    bool heading_forward_;

    bool heading_backward_;
    bool heading_backward_due_to_collision_;
    bool already_thrusting_;
    Sound *sound_effect_;
};

} // namespace cute
