#include "AnimationAttack.h"
#include "EntitySprite.h"
#include "Map.h"
#include "PositionalSound.h"
#include "Slot.h"
#include "TopDownSprite.h"
#include "Utilities.h"

using namespace cute;

AnimationAttack::AnimationAttack(const std::string &animation_to_play_on_attack,
                                 const std::string &sound_to_play_on_attack, int damage, double arc_range,
                                 double arc_angle)
        : animation_to_play_on_attack_(animation_to_play_on_attack), sound_to_play_on_attack_(sound_to_play_on_attack),
          damage_(damage), arch_range_(arc_range), arc_angle_(arc_angle) {

    /// empty sprite
    set_sprite(new TopDownSprite(QPixmap()));
    set_icon(QPixmap(":/cute-engine-builtin/resources/graphics/misc/axeicon.png"));
}

void AnimationAttack::attack(QPointF position) {
    /// do nothing if were already attacking
    if (already_attacking_) {
        return;
    }
    Entity *owner = slot_equipped_in()->owner();
    assert(owner != nullptr);
    Map *owners_map = owner->map();
    assert(owners_map != nullptr);
    EntitySprite *owner_sprite = owner->sprite();
    assert(owner_sprite != nullptr);

    /// play sound
    sound_effect_ = new PositionalSound(owners_map, sound_to_play_on_attack_, QPointF());
    sound_effect_->set_pos(owner->pos());
    sound_effect_->play(1);

    /// listen to when the animation reaches nth frame (when we'll apply the damage)
    connect(owner_sprite, &EntitySprite::frame_switched, this, &AnimationAttack::on_frame_switched);

    /// listen to when the animation finishes (so we know we are done attacking)
    connect(owner_sprite, &EntitySprite::animation_finished, this, &AnimationAttack::on_owner_animation_finished);

    /// play animation
    already_attacking_ = true;
    owner_sprite->play_then_go_back_to_old_animation(animation_to_play_on_attack_, 1, 10, 0);
}

/// Executed when the owner's sprite's frames are being switched.
/// We will see if we are on the nth frame and if so
/// we will damage entities in an arc and then stop listening to this event.
void AnimationAttack::on_frame_switched(EntitySprite *owner_sprite, int from_frame_num, int to_frame_num) {
    if (to_frame_num != attacking_frame_number_) {
        return;
    }

    Entity *owner = slot_equipped_in()->owner();
    // EntitySprite *owner_sprite = owner->sprite();
    owner_sprite = owner->sprite();

    /// get everyone in arc and damage them
    Map *entitys_map = owner->map();
    QPolygonF poly;
    QPointF owners_pos = owner->pos();
    poly.append(owners_pos);
    QLineF line(owners_pos, QPointF(0, 0));
    line.setAngle(owner->facing_angle() * -1);
    line.setLength(arch_range_);
    line.setAngle(line.angle() + arc_angle_ / 2);
    poly.append(line.p2());
    line.setAngle(line.angle() - arc_angle_);
    poly.append(line.p2());

    /// DEBUG, enable this to visualize attack area
    // owner->map()->scene()->addPolygon(poly);

    std::unordered_set<Entity *> entities_in_region = entitys_map->entities(poly);
    for (Entity *e : entities_in_region) {
        if (e != this && e != owner) {
            owner->damage_enemy(e, damage_);
        }
    }

    /// stop listening to frame switched (will relisten on new attack)
    disconnect(owner_sprite, &EntitySprite::frame_switched, this, &AnimationAttack::on_frame_switched);
}

void AnimationAttack::on_owner_animation_finished(EntitySprite *ownerSprite, std::string animation) {
    Entity *owner = slot_equipped_in()->owner();
    EntitySprite *owners_sprite = owner->sprite();

    already_attacking_ = false;
    disconnect(owners_sprite, &EntitySprite::animation_finished, this, &AnimationAttack::on_owner_animation_finished);
}
