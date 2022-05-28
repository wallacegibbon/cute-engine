#pragma once

#include "Weapon.h"

class QPointF;

namespace cute {

/// Abstract class that represents a weapon that can fire projectiles.
class RangedWeapon : public Weapon {
public:
    /// Returns the point at which projectiles will be spawn.
    /// This point is in local coordinates (relative to the RangedWeapon itself).
    QPointF projectile_spawn_point() { return projectile_spawn_point_; }
    void set_projectile_spawn_point(QPointF point) { projectile_spawn_point_ = point; }
    void reset_projectile_spawn_point();

private:
    QPointF projectile_spawn_point_;
};

} // namespace cute
