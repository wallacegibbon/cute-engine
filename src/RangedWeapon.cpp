#include "RangedWeapon.h"
#include "EntitySprite.h"
#include "Sprite.h"

using namespace cute;

/// Resets the projectile spawn point to be at the very center of the RangedWeapon's sprite.
void RangedWeapon::reset_projectile_spawn_point() {
    double length = sprite()->currently_displayed_frame().width();
    double width = sprite()->currently_displayed_frame().height();

    QPointF center;
    center.setX(length / 2);
    center.setY(width / 2);
    set_projectile_spawn_point(center);
}
