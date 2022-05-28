#include "ShardsOfFireAbility.h"
#include "ECSineMover.h"
#include "Map.h"
#include "Sound.h"
#include "SpearProjectile.h"
#include "TopDownSprite.h"

cute::ShardsOfFireAbility::ShardsOfFireAbility(int num_shards, double shard_distance, cute::Entity *owner)
        : NoTargetAbility(owner), num_shards_(num_shards), shard_distance_(shard_distance) {
    set_icon(QPixmap(":/cute-engine-builtin/resources/graphics/effects/fire_rain.png"));
    set_description("Expells shards of fire from the owner.");
    sound_effect_ = new Sound("qrc:/cute-engine-builtin/resources/sounds/fireball.wav");
}

void cute::ShardsOfFireAbility::use_implementation() {
    sound_effect_->play(1);

    const double angle_step = 360.0 / num_shards_;

    /// create projectiles
    std::vector<SpearProjectile *> projectiles;
    for (int i = 0, n = num_shards_; i < n; i++) {
        SpearProjectile *projectile = new SpearProjectile(shard_distance_, 10);
        projectile->set_sprite(
                new TopDownSprite(QPixmap(":/cute-engine-builtin/resources/graphics/effects/fireball.png")));
        projectile->set_origin(QPointF(0, 0));
        owner()->map()->add_entity(projectile);
        projectiles.push_back(projectile);
    }

    /// make sure projectiles don't damage each other (or owner)
    for (SpearProjectile *p : projectiles) {
        p->add_entity_to_not_collide_with(owner());
        for (SpearProjectile *op : projectiles) {
            p->add_entity_to_not_collide_with(op);
        }
    }

    /// launch projectiles
    for (int i = 0, n = num_shards_; i < n; i++) {
        double angle = i * angle_step;
        QLineF line(owner()->pos(), QPoint(0, 0));
        line.setAngle(angle);
        line.setLength(shard_distance_);

        SpearProjectile *projectile = projectiles[i];
        projectile->set_pos(owner()->pos());
        projectile->set_facing_angle(line.angle());
        projectile->shoot_towards(line.p2());
    }
}
