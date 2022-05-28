#pragma once

#include "Projectile.h"

class QPointF;

namespace cute {

class Map;

/// Represents a Projectile that simply goes from the start position
/// towards the target position until it exceeds its range. When the SpearProjectile
/// exceeds its range, it will simply be destroyed.
/// @author Abdullah Aghazadah
/// @date 5/24/16
class SpearProjectile : public Projectile {
public:
    SpearProjectile(double range, double damage);
    void shoot_towards(const QPointF &pos) override;

private:
    double range_;
    double dist_travelled_so_far_ = 0;
};

} // namespace cute
