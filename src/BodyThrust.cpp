#include "BodyThrust.h"
#include "Entity.h"
#include "EntitySprite.h"
#include "Inventory.h"
#include "Map.h"
#include "QTimer"
#include "Sound.h"
#include "Sprite.h"

using namespace cute;

BodyThrust::BodyThrust(Entity *owner) : NoTargetAbility(owner) {
    set_thrust_distance(65);
    set_thrust_speed(250);

    timer_ = new QTimer(this);
    reset_variables();

    sound_effect_ = new Sound("qrc:/cute-engine-builtin/resources/sounds/spear.wav");

    set_owner(owner);
}

void BodyThrust::use_implementation() {
    /// before using the ability, make sure owner of ability is in a map
    Entity *the_owner = owner();
    assert(the_owner->map() != nullptr);

    /// set point that will be checked for collision
    double owner_width = the_owner->sprite()->bounding_box().width();
    double owner_height = the_owner->sprite()->bounding_box().height();
    collision_point_ = QPointF(64, 64);

    /// if its already thrusting, don't do anything
    if (already_thrusting_) {
        return;
    }

    sound_effect_->play(1);

    damaged_ = false;
    heading_backward_ = false;
    heading_forward_ = true;
    current_thrust_steps_ = 0;
    connect(timer_, &QTimer::timeout, this, &BodyThrust::thrust_step);
    timer_->start(thrust_step_frequency_);
    already_thrusting_ = true;

    if (animation_to_play_ != "") {
        last_anim_ = owner()->sprite()->playing_animation();
        owner()->sprite()->play(animation_to_play_, -1, 10, 0);
    }
}

void BodyThrust::set_thrust_speed(double speed) {
    /// thrust speed is set by changing thrustFrequency_
    double thrustsPerSecond = speed / thrust_length_each_step_;
    thrust_step_frequency_ = 1000 * (1.0 / thrustsPerSecond);
    thrust_speed_ = thrust_length_each_step_ * thrustsPerSecond;
}

void BodyThrust::set_thrust_distance(double distance) {
    /// thrust distance is set by changing the max_thrust_steps_
    int num_of_thrusts = distance / thrust_length_each_step_;
    max_thrust_steps_ = num_of_thrusts;
    thrust_distance_ = thrust_length_each_step_ * num_of_thrusts;
}

void BodyThrust::thrust_step() {
    const int EXTRA_BACK_STEPS = 0;

    /// if the entity to thrust is dead, were done
    Entity *the_owner = owner();
    if (the_owner == nullptr) {
        done_();
        return;
    }

    /// if the entity to thrust is no longer in a map, were done
    Map *owners_map = the_owner->map();
    if (owners_map == nullptr) {
        done_();
        return;
    }

    /// if moved backward enough, stop moving
    if (heading_backward_ && current_thrust_steps_ >= max_thrust_steps_ + EXTRA_BACK_STEPS) {
        done_();
        return;
    }

    /// if moved backward enough due to collision, stop
    if (heading_backward_due_to_collision_ && current_thrust_steps_ == 0) {
        done_();
        return;
    }

    /// if still moving forward, damage things in the way, (then move backward) <- don't do the move backward yet
    /// (over-inflated bboxes won't let this work properly)
    std::unordered_set<Entity *> colliding_entities = owners_map->entities(the_owner->map_to_map(collision_point_));
    for (Entity *e : colliding_entities) {
        if (e != the_owner && e->parent() != the_owner && heading_forward_ && !damaged_) {
            the_owner->damage_enemy(e, damage_);
            damaged_ = true;
            // heading_backward_due_to_collision_ = true;
            // heading_backward_ = false;
            // heading_forward_ = false;
        }
    }

    /// if heading backward due to collision, move backward
    if (heading_backward_due_to_collision_ && current_thrust_steps_ > 0) {
        /// move owner backward at current angle
        QLineF line(the_owner->pos(), the_owner->pos() + QPointF(1, 1));
        line.setAngle(360 - the_owner->facing_angle());
        line.setAngle(line.angle() + 180);
        line.setLength(thrust_length_each_step_);
        double new_x = the_owner->pos().x() + line.dx();
        double new_y = the_owner->pos().y() + line.dy();
        QPointF new_pt(new_x, new_y);
        the_owner->set_pos(new_pt);

        current_thrust_steps_--;
        return;
    }

    /// if moving forward, move forward
    if (heading_forward_ && current_thrust_steps_ < max_thrust_steps_) {
        /// move owner forward at current angle
        QLineF line(the_owner->pos(), the_owner->pos() + QPointF(1, 1));
        line.setAngle(360 - the_owner->facing_angle());
        line.setLength(thrust_length_each_step_);
        double new_x = the_owner->pos().x() + line.dx();
        double new_y = the_owner->pos().y() + line.dy();
        QPointF new_pt(new_x, new_y);
        the_owner->set_pos(new_pt);

        /// update thrust step counter
        current_thrust_steps_++;

        return;
    }

    /// if moving backward, move backward
    if (heading_backward_ && current_thrust_steps_ < max_thrust_steps_ + EXTRA_BACK_STEPS) {
        /// move owner backward at current angle
        QLineF line(the_owner->pos(), the_owner->pos() + QPointF(1, 1));
        line.setAngle(360 - the_owner->facing_angle());
        line.setAngle(line.angle() + 180);
        line.setLength(thrust_length_each_step_);
        double new_x = the_owner->pos().x() + line.dx();
        double new_y = the_owner->pos().y() + line.dy();
        QPointF new_pt(new_x, new_y);
        the_owner->set_pos(new_pt);

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

void BodyThrust::reset_variables() {
    heading_backward_ = false;
    heading_forward_ = false;
    heading_backward_due_to_collision_ = false;
    already_thrusting_ = false;
    damaged_ = false;
}

void BodyThrust::done_() {
    reset_variables();
    timer_->disconnect();
    if (animation_to_play_ == "" || last_anim_.is_none()) {
        return;
    }
    owner()->sprite()->play(last_anim_.name(), last_anim_.times_left_to_play(), last_anim_.fps(),
                            last_anim_.current_frame());
}
