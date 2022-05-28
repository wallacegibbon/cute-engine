#include "FireballLauncher.h"
#include "CBDamage.h"
#include "Inventory.h"
#include "Map.h"
#include "Sound.h"
#include "SpearProjectile.h"
#include "TopDownSprite.h"

using namespace cute;

FireballLauncher::FireballLauncher() {
    TopDownSprite *spr = new TopDownSprite(QPixmap(":/cute-engine-builtin/resources/graphics/effects/fireball.png"));
    set_sprite(spr);
    set_description("A Weapon that when used will launch fireballs at the clicked location.");

    /// default projectile spawn point (center of sprite)
    reset_projectile_spawn_point();

    QPointF pt;
    pt.setX(spr->currently_displayed_frame().width() / 2);
    pt.setY(spr->currently_displayed_frame().height() / 2);
    set_attachment_point(pt);

    sound_effect_ = new Sound("qrc:/cute-engine-builtin/resources/sounds/fireball.wav", this);
    set_cast_range(400);
}

void FireballLauncher::attack(QPointF position) {
    sound_effect_->play(1);

    Entity *owner = inventory()->owner();
    assert(owner != nullptr);

    Map *map = owner->map();
    assert(map != nullptr);

    /// create a spear projectile
    QPointF start_pos = map_to_map(projectile_spawn_point());

    SpearProjectile *fireball_projectile = new SpearProjectile(600, 10);

    /// do not collide with self or owner
    fireball_projectile->add_entity_to_not_collide_with(this);
    fireball_projectile->add_entity_to_not_collide_with(owner);

    fireball_projectile->set_sprite(
            new TopDownSprite(QPixmap(":/cute-engine-builtin/resources/graphics/effects/fireball.png")));

    map->add_entity(fireball_projectile);

    fireball_projectile->set_pos(start_pos);
    fireball_projectile->shoot_towards(position);
    fireball_projectile->set_z(owner->z() + owner->height());
}
