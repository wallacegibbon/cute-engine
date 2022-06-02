#include "RainOfSpearsAbility.h"
#include "CBDamage.h"
#include "Map.h"
#include "RandomGenerator.h"
#include "Sound.h"
#include "SpearProjectile.h"
#include "Sprite.h"

using namespace cute;

RainOfSpearsAbility::RainOfSpearsAbility(Entity *owner) : NoTargetAbility(owner) {
    set_icon(QPixmap(":/cute-engine-builtin/resources/graphics/weapons/tripple_spear.png"));
    set_description("Rains spears around the owner. The spears damage enemies of the owner.");
    timer_ = new QTimer(this);
    sound_effect_ = new Sound("qrc:/cute-engine-builtin/resources/sounds/special_move.mp3", this);
}

void cute::RainOfSpearsAbility::use_implementation() {
    connect(timer_, &QTimer::timeout, this, &RainOfSpearsAbility::on_spear_step);
    timer_->start(500);
    sound_effect_->play(1);
    times_ = 0;
}

void RainOfSpearsAbility::on_spear_step() {
    const int NUM_SPEARS_TO_GENERATE_PER_WAVE = 5;
    const int NUM_WAVES = 15;

    for (int i = 0; i < NUM_SPEARS_TO_GENERATE_PER_WAVE; i++) {
        one_more_spear();
    }
    times_++;
    if (times_ >= NUM_WAVES) {
        timer_->disconnect();
    }
}

void RainOfSpearsAbility::one_more_spear() {
    static RandomGenerator generator;
    /// number of pixels around the owner's x position to spawn projectiles at
    const int X_RADIUS = 1000;
    /// number of pixels ABOVE the owner to spawn projectiles
    const int Y_OFFSET = 1000;
    /// the y value "thickness" of possible y locations projectiles can spawn
    const int Y_RADIUS = 600;

    int random_x_offset = generator.rand_int(-X_RADIUS / 2, X_RADIUS / 2);
    int random_y_offset = generator.rand_int(-Y_OFFSET, Y_RADIUS - Y_OFFSET);
    QPointF random_pos(owner()->pos().x() + random_x_offset, owner()->pos().y() + random_y_offset);

    QPointF target_pos = random_pos;
    target_pos.setY(target_pos.y() + 20);
    target_pos.setX(target_pos.x());

    SpearProjectile *spear_projectile = new SpearProjectile(800, 5);
    spear_projectile->add_entity_to_not_collide_with(owner());

    owner()->map()->add_entity(spear_projectile);

    spear_projectile->set_pos(random_pos);
    spear_projectile->shoot_towards(target_pos);
}