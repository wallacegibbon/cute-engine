#pragma once

#include "MeleeWeapon.h"
#include "Vendor.h"

class QTimer;

namespace cute {

class Sound;

/// An Axe is a MeleeWeapon that swings and executes its CollisionBehavior when it hits stuff.
class Axe : public MeleeWeapon {
    Q_OBJECT

public:
    Axe();
    void attack(QPointF position) override;

public slots:
    void swing_step();

private:
    QTimer *timer_;

    /// has the weapon already begun swinging?
    bool already_swinging_ = false;

    double swing_angle_each_step_ = 5;

    /// how often we step
    int swing_frequency_ = 19;

    /// how many steps to initially swing out
    int max_draw_back_steps_ = 9;

    int current_draw_back_steps_;
    int max_draw_forward_steps_ = 9;
    int current_draw_forward_steps_;

    /// how many steps to go after coming back from draw
    int max_forward_steps_ = 7;

    int current_forward_steps_;
    int max_backward_steps_ = 7;
    int current_backward_steps_;

    /// if we hit someting going forward, how many steps till we get back to neutral?
    int steps_to_go_backward_to_neutral_;

    int current_step_to_going_back_to_neural_;

    bool hit_something_coming_back_from_draw_;
    bool hit_something_during_forward_step_;

    Sound *sound_effect_;
};

} // namespace cute
