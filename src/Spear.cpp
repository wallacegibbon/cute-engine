#include "Spear.h"
#include "CBDamage.h"
#include "Entity.h"
#include "Inventory.h"
#include "Map.h"
#include "Sound.h"
#include "Sprite.h"
#include "TopDownSprite.h"

using namespace cute;

void Spear::reset_variables() {
    heading_backward_ = false;
    heading_forward_ = false;
    heading_backward_due_to_collision_ = false;
    already_thrusting_ = false;
}

Spear::Spear() {
    /// set collision behavior
    set_collision_behavior(new CBDamage(0, 10));

    set_thrust_distance(45);
    set_thrust_speed(250);

    /// default sprite
    EntitySprite *spr = new TopDownSprite(QPixmap(":/cute-engine-builtin/resources/graphics/weapons/spear.png"));
    set_sprite(spr);

    /// default tip (length and width/2)
    reset_tip();

    /// default attachment point
    QPointF pt;
    pt.setX(spr->currently_displayed_frame().width() / 3);
    pt.setY(spr->currently_displayed_frame().height() / 2);
    set_attachment_point(pt);

    timer_ = new QTimer(this);
    reset_variables();

    sound_effect_ = new Sound("qrc:/cute-engine-builtin/resources/sounds/spear.wav", this);
}

/// Will thrust the spear forward. The "position" argument is ignored.
void Spear::attack(QPointF position) {
    /// if its already thrusting, don't do anything
    if (already_thrusting_) {
        return;
    }

    sound_effect_->play(1);

    heading_backward_ = false;
    heading_forward_ = true;
    current_thrust_steps_ = 0;
    connect(timer_, &QTimer::timeout, this, &Spear::thrust_step);
    timer_->start(thrust_step_frequency_);
    already_thrusting_ = true;
}

/// Sets how fast the spear thrusts at (in pixels pers second).
void Spear::set_thrust_speed(double speed) {
    /// thrust speed is set by changing thrustFrequency_
    double thrustsPerSecond = speed / thrust_length_each_step_;
    thrust_step_frequency_ = 1000 * (1.0 / thrustsPerSecond);
    thrust_speed_ = thrust_length_each_step_ * thrustsPerSecond;
}

/// Sets how far the spear thrusts (in pixels). Also sets the cast range accordingly.
void Spear::set_thrust_distance(double distance) {
    /// thrust distance is set by changing the max_thrust_steps_
    int num_of_thrusts = distance / thrust_length_each_step_;
    max_thrust_steps_ = num_of_thrusts;
    thrust_distance_ = thrust_length_each_step_ * num_of_thrusts;

    set_cast_range(thrust_distance_);
}

void Spear::thrust_step() {
    /// if moved backward enough, stop moving
    if (heading_backward_ && current_thrust_steps_ >= max_thrust_steps_) {
        timer_->disconnect();

        reset_variables();
        return;
    }

    /// if moved backward enough due to collision, stop
    if (heading_backward_due_to_collision_ && current_thrust_steps_ == 0) {
        timer_->disconnect();

        reset_variables();
        return;
    }

    /// if still moving forward, kill things with tip, then move backward due to collision
    std::unordered_set<Entity *> colliding_entities = map()->entities(map_to_map(tip()));
    Entity *owner = inventory()->owner();
    for (Entity *e : colliding_entities) {
        if (e != this && e != owner && e->parent() != owner && heading_forward_) {
            /// let collision behavior handle collision
            collision_behavior()->on_collided(this, e, {}, {});
            heading_backward_due_to_collision_ = true;
            heading_backward_ = false;
            heading_forward_ = false;
        }
    }

    /// if heading backward due to collision, move backward
    if (heading_backward_due_to_collision_ && current_thrust_steps_ > 0) {
        /// move spear backward at current angle
        QLineF line(pos(), QPointF(1, 1));
        line.setAngle(360 - facing_angle());
        line.setAngle(line.angle() + 180);
        line.setLength(thrust_length_each_step_);
        double new_x = pos().x() + line.dx();
        double new_y = pos().y() + line.dy();
        QPointF new_pt(new_x, new_y);
        set_pos(new_pt);

        current_thrust_steps_--;
        return;
    }

    /// if moving forward, move forward
    if (heading_forward_ && current_thrust_steps_ < max_thrust_steps_) {
        /// move spear forward at current angle
        QLineF line(pos(), QPointF(1, 1));
        line.setAngle(360 - facing_angle());
        line.setLength(thrust_length_each_step_);
        double new_x = pos().x() + line.dx();
        double new_y = pos().y() + line.dy();
        QPointF new_pt(new_x, new_y);
        set_pos(new_pt);

        /// update thrust step counter
        current_thrust_steps_++;

        return;
    }

    /// if moving backward, move backward
    if (heading_backward_ && current_thrust_steps_ < max_thrust_steps_) {
        /// move spear backward at current angle
        QLineF line(pos(), QPointF(1, 1));
        line.setAngle(360 - facing_angle());
        line.setAngle(line.angle() + 180);
        line.setLength(thrust_length_each_step_);
        double new_x = pos().x() + line.dx();
        double new_y = pos().y() + line.dy();
        QPointF new_pt(new_x, new_y);
        set_pos(new_pt);

        /// update thrust step counter
        current_thrust_steps_++;

        return;
    }

    /// if moved forward enough, move backward
    if (heading_forward_ && current_thrust_steps_ >= max_thrust_steps_) {
        heading_forward_ = false;
        heading_backward_ = true;
        current_thrust_steps_ = 0;

        return;
    }
}
