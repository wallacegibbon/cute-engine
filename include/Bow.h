#pragma once

#include "RangedWeapon.h"
#include "Vendor.h"

class QPointF;

namespace cute {

class Sound;

class Bow : public RangedWeapon {
public:
    Bow();
    void attack(QPointF target_point);

private:
    Sound *sound_effect_;
};

} // namespace cute
