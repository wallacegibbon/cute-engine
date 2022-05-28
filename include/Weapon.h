#pragma once

#include "EquipableItem.h"

namespace cute {

/// An abstract class that represents an EquipableItem that can attack towards a position.
class Weapon : public EquipableItem {
public:
    virtual void attack(QPointF position) = 0;

    double cast_range() { return cast_range_; }
    void set_cast_range(double range) { cast_range_ = range; }

private:
    /// how far can this weapon be used from?
    double cast_range_ = 100;
};

} // namespace cute
