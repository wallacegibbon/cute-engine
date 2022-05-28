#include "MeleeWeapon.h"
#include "EntitySprite.h"
#include "Inventory.h"
#include "Map.h"
#include "Sprite.h"

using namespace cute;

void MeleeWeapon::reset_tip() {
    QPointF pt;
    pt.setX(sprite()->currently_displayed_frame().width());
    pt.setY(sprite()->currently_displayed_frame().height() / 2);
    set_tip(pt);
}

void MeleeWeapon::set_collision_behavior(CollisionBehavior *collision_behavior) {
    /// release old collision behavior
    collision_behavior_.release();
    /// grab new one
    collision_behavior_.reset(collision_behavior);
}

CollisionBehavior *MeleeWeapon::collision_behavior() { return collision_behavior_.get(); }
