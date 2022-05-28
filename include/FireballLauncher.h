#pragma once

#include "RangedWeapon.h"
#include "Vendor.h"

class QPointF;

namespace cute {

class Sound;

/// A RangedWeapon that on attack() will launch a fireball towards the specified position.
class FireballLauncher : public RangedWeapon {
public:
    FireballLauncher();
    void attack(QPointF position);

private:
    Sound *sound_effect_;
};

} // namespace cute
