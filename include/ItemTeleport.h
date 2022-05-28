#pragma once

#include "PointTargetItem.h"
#include "Vendor.h"

namespace cute {

class Sound;

class ItemTeleport : public PointTargetItem {
public:
    ItemTeleport();
    void use_(const QPointF &point) override;

private:
    Sound *sound_effect_;
};

} // namespace cute
