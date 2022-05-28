#pragma once

#include "MeleeWeapon.h"
#include "Vendor.h"

namespace cute {

class PositionalSound;

/// A MeleeWeapon that on attack() will cause a specified animation of the owner to be played.
/// Additionally, all entities that are within a specified arc will be damaged by a specified amount.
/// @author Abdullah Aghazadah
/// @date 11/3/17
class AnimationAttack : public MeleeWeapon {
    Q_OBJECT

public:
    AnimationAttack(const std::string &animation_to_play_on_attack, const std::string &sound_to_play_on_attack,
                    int damage, double arc_range, double arc_angle);

    void attack(QPointF position);

public slots:
    void on_frame_switched(EntitySprite *owners_sprite, int from_frame_num, int to_frame_num);
    void on_owner_animation_finished(EntitySprite *ownerSprite, std::string animation);

private:
    std::string animation_to_play_on_attack_;
    std::string sound_to_play_on_attack_;
    int damage_;
    double arch_range_;
    double arc_angle_;

    PositionalSound *sound_effect_ = nullptr;
    bool already_attacking_ = false;

    /// the frame number to actually attack on (create arc and damage entities)
    int attacking_frame_number_ = 0;
};

} // namespace cute
