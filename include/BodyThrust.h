#pragma once

#include "NoTargetAbility.h"
#include "PlayingAnimationInfo.h"
#include "Vendor.h"

class QTimer;

namespace cute {

class Sound;

/// An Ability that causes the using entity to thrust its body forward at its current
/// facing angle and then back. You can optionally specify an animation of the Entity
/// that will be played when its thrusting.
/// @author Abdullah Aghazadah
/// @date 11/18/16
class BodyThrust : public NoTargetAbility {
    Q_OBJECT

public:
    BodyThrust(Entity *owner);

    virtual void use_implementation();

    double thrust_distance() { return thrust_distance_; }
    double thrust_speed() { return thrust_speed_; }
    void set_thrust_speed(double speed);
    void set_thrust_distance(double distance);
    bool thrusting() const { return already_thrusting_; }

    /// Sets the animation that should play when the owner is thrusting forward.
    /// Pass in "" to have no animation play (leave w.e. animation the owner has at the time).
    void set_animation_to_play_while_thrusting(const std::string &animation) { animation_to_play_ = animation; }

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

    /// the number of pixels the owner will thrust.
    /// calculate from other attributes, here for perf
    double thrust_distance_;

    /// the number of pixels per second the owner will thrust at.
    /// calculated from other attributes, here for perf
    double thrust_speed_;

    bool damaged_;

    /// is the spear heading forward or backward?
    bool heading_forward_;
    bool heading_backward_;

    bool heading_backward_due_to_collision_;
    bool already_thrusting_;
    QPointF collision_point_;

    double damage_ = 5;
    std::string animation_to_play_;
    PlayingAnimationInfo last_anim_;

    Sound *sound_effect_;

    void done_();
};

} // namespace cute
