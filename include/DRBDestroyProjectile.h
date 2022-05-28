#pragma once

#include "DestReachedBehavior.h"

namespace cute {

/// A DestReachedBehavior that simply destroys the projectile.
class DRBDestroyProjectile : public DestReachedBehavior {
public:
    void on_destination_reached(Projectile &projectile) override;
};

} // namespace cute
