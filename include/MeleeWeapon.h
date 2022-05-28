#pragma once

#include "CollisionBehavior.h"
#include "Vendor.h"
#include "Weapon.h"

class QPointF;

namespace cute {

class MeleeWeapon : public Weapon {
public:
    QPointF tip() { return tip_; }
    void set_tip(QPointF point) { tip_ = point; }
    void reset_tip();

    void set_collision_behavior(CollisionBehavior *collision_behavior);
    CollisionBehavior *collision_behavior();

private:
    /// The tip is the point that will be check for collision with things.
    QPointF tip_;
    std::unique_ptr<CollisionBehavior> collision_behavior_;
};

} // namespace cute
