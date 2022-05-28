#include "Axe.h"
#include "CollisionBehavior.h"
#include "Entity.h"
#include "Inventory.h"
#include "Map.h"
#include "Sound.h"
#include "Sprite.h"
#include "TopDownSprite.h"

using namespace cute;

Axe::Axe() {
    /// default sprite
    TopDownSprite *spr = new TopDownSprite(QPixmap(":/cute-engine-builtin/resources/graphics/weapons/axe.png"));
    set_sprite(spr);

    /// default tip (spriteWidth and spriteHeight/2)
    reset_tip();

    /// default attachment point
    QPointF pt;
    pt.setX(spr->currently_displayed_frame().width() / 3);
    pt.setY(spr->currently_displayed_frame().height() / 2);
    set_attachment_point(pt);

    /// default cast range
    set_cast_range(100);

    timer_ = new QTimer(this);

    sound_effect_ = new Sound("qrc:/cute-engine-builtin/resources/sounds/axe.wav", this);
}

/// The axe will start swinging. The "position" argument is ignored.
void Axe::attack(QPointF position) {
    /// if its already swinging, don't do anything
    if (already_swinging_) {
        return;
    }

    sound_effect_->play(1);

    /// set variables for brand new swing
    current_draw_back_steps_ = 0;
    current_draw_forward_steps_ = 0;
    current_forward_steps_ = 0;
    current_backward_steps_ = 0;
    current_step_to_going_back_to_neural_ = 0;
    hit_something_coming_back_from_draw_ = false;
    hit_something_during_forward_step_ = false;

    /// start swinging
    connect(timer_, &QTimer::timeout, this, &Axe::swing_step);
    timer_->start(swing_frequency_);
    already_swinging_ = true;
}

void Axe::swing_step() {
    /// if we have hit something going forward
    if (hit_something_during_forward_step_) {
        set_facing_angle(facing_angle() - swing_angle_each_step_);
        current_step_to_going_back_to_neural_++;
        if (current_step_to_going_back_to_neural_ >= steps_to_go_backward_to_neutral_) {
            timer_->disconnect();
            already_swinging_ = false;
            return;
        }
        return;
    }

    /// if we have hit something coming back from draw
    if (hit_something_coming_back_from_draw_) {
        /// keep moving towards neutral position (w/o damaging) and rest there
        set_facing_angle(facing_angle() + swing_angle_each_step_);
        current_draw_forward_steps_++;
        if (current_draw_forward_steps_ >= max_draw_forward_steps_) {
            timer_->disconnect();
            already_swinging_ = false;
        }
        return;
    }

    /// if initially drawing
    if (current_draw_back_steps_ < max_draw_back_steps_) {
        set_facing_angle(facing_angle() - swing_angle_each_step_);
        current_draw_back_steps_++;
        return;
    }

    /// if coming back from draw
    if (current_draw_forward_steps_ < max_draw_forward_steps_) {
        set_facing_angle(facing_angle() + swing_angle_each_step_);
        current_draw_forward_steps_++;

        /// if hit something
        Entity *owner = inventory()->owner();
        std::unordered_set<Entity *> colliding_entities = map()->entities(map_to_map(tip()));
        for (Entity *e : colliding_entities) {
            if (e != this && e != owner && e->parent() != owner) {
                collision_behavior()->on_collided(this, e, {}, {});
                hit_something_coming_back_from_draw_ = true;
                return;
            }
        }
        return;
    }

    /// if forward step
    if (current_forward_steps_ < max_forward_steps_) {
        set_facing_angle(facing_angle() + swing_angle_each_step_);
        current_forward_steps_++;

        /// if hit something
        Entity *owner = inventory()->owner();
        std::unordered_set<Entity *> colliding_entities = map()->entities(map_to_map(tip()));
        for (Entity *e : colliding_entities) {
            if (e != this && e != owner && e->parent() != owner) {
                collision_behavior()->on_collided(this, e, {}, {});
                hit_something_during_forward_step_ = true;
                steps_to_go_backward_to_neutral_ = current_forward_steps_;
                return;
            }
        }
        return;
    }

    /// if backward step
    if (current_backward_steps_ < max_backward_steps_) {
        set_facing_angle(facing_angle() - swing_angle_each_step_);
        current_backward_steps_++;

        /// if last backward step, stop
        if (current_backward_steps_ >= max_backward_steps_) {
            timer_->disconnect();
            already_swinging_ = false;
        }
        return;
    }
}
