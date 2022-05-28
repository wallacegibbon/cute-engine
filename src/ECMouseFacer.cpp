#include "ECMouseFacer.h"
#include "ECRotater.h"
#include "Map.h"

using namespace cute;

ECMouseFacer::ECMouseFacer(Entity *entity) : EntityController(entity) {
    rotater_ = new ECRotater(entity);
    rotater_->setParent(this);

    rotate_timer_ = new QTimer(this);
    connect(rotate_timer_, &QTimer::timeout, this, &ECMouseFacer::on_rotate_step);
    rotate_timer_->start(rotate_frequency_);
}

void ECMouseFacer::on_rotate_step() {
    Entity *entity = entity_controlled();
    /// if the entity has been destroyed, stop rotating
    if (entity == nullptr) {
        rotate_timer_->disconnect();
        return;
    }

    /// do nothing if entity is not in a map
    Map *entitys_map = entity->map();
    if (entitys_map == nullptr) {
        return;
    }
    /// do nothing if the entity's map isn't game's current map
    if (entitys_map->game() == nullptr) {
        return;
    }

    /// using rotater is slower but more natural
    // rotater_->rotate_towards(entitys_map->get_mouse_position());

    /// this will be faster
    QLineF entityToMousePos(entity->pos(), entitys_map->get_mouse_position());
    entity->set_facing_angle(-entityToMousePos.angle());
}
