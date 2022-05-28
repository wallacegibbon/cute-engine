#include "Bow.h"
#include "CBDamage.h"
#include "Entity.h"
#include "Inventory.h"
#include "Map.h"
#include "Projectile.h"
#include "Sound.h"
#include "SpearProjectile.h"
#include "Sprite.h"
#include "TopDownSprite.h"

using namespace cute;

Bow::Bow() {
    TopDownSprite *spr = new TopDownSprite(QPixmap(":/cute-engine-builtin/resources/graphics/weapons/bow.png"));
    set_sprite(spr);

    /// default projectile spawn point (center of sprite)
    reset_projectile_spawn_point();

    QPointF pt;
    pt.setX(spr->currently_displayed_frame().width() / 2);
    pt.setY(spr->currently_displayed_frame().height() / 2);
    set_attachment_point(pt);

    sound_effect_ = new Sound("qrc:/cute-engine-builtin/resources/sounds/bow.wav", this);
    set_cast_range(400);
}

/// Spawns a projectile towards the target_point. target_point is in map coordinates.
void Bow::attack(QPointF target_point) {
    sound_effect_->play(1);

    Entity *owner = inventory()->owner();
    assert(owner != nullptr);

    Map *map = owner->map();
    assert(map != nullptr);

    /// create a spear projectile
    QPointF start_pos = map_to_map(projectile_spawn_point());

    SpearProjectile *spear_projectile = new SpearProjectile(600, 50);

    /// do not collide with bow or the owner
    spear_projectile->add_entity_to_not_collide_with(this);
    spear_projectile->add_entity_to_not_collide_with(owner);

    map->add_entity(spear_projectile);

    spear_projectile->set_pos(start_pos);
    spear_projectile->shoot_towards(target_point);
    spear_projectile->set_z(owner->z() + owner->height());
}
