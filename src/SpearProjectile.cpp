#include "SpearProjectile.h"
#include "CBDamage.h"
#include "DRBDestroyProjectile.h"
#include "ECStraightMover.h"
#include "Sprite.h"
#include "TopDownSprite.h"
#include "Utilities.h"
#include <QPixmap>

using namespace cute;

SpearProjectile::SpearProjectile(double range, double damage)
        : Projectile(nullptr, nullptr, new DRBDestroyProjectile()), range_(range) {
    set_sprite(new TopDownSprite(QPixmap(":/cute-engine-builtin/resources/graphics/weapons/spear.png")));
    set_speed(1000);

    ECStraightMover *sm = new ECStraightMover(this);
    set_mover(sm);

    /// set CollisionBehavior
    CBDamage *cb = new CBDamage(health() + 1, damage);
    set_collision_behavior(cb);

    set_origin(QPointF(0, sprite()->bounding_box().height() / 2));
}

void SpearProjectile::shoot_towards(const QPointF &target_pos) {
    QLineF shoot_line(pos(), target_pos);
    shoot_line.setLength(range_);

    /// delegate to base
    Projectile::shoot_towards(shoot_line.p2());
}
