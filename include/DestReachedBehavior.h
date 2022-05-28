#pragma once

namespace cute {

class Projectile;

/// An object that determines what happens when a Projectile has reached its destination.
///
/// Concrete Projectiles may destroy the projectile, explode it, etc...

class DestReachedBehavior {
public:
    virtual void on_destination_reached(Projectile &projectile) = 0;
};

} // namespace cute
