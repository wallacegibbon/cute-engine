#include "ECMover.h"

using namespace cute;

void ECMover::set_entity(Entity *entity) {
    /// cannot call set_entity() while Mover is already moving an Entity
    assert(!is_moving_entity());

    /// TODO: is it necessary to call disconnect? when the previous entity dies, won't the connection be lost ?
    // disconnect(0, &Entity::dying, this, &ECMover::on_entity_died);
    connect(entity, &Entity::dying, this, &ECMover::on_entity_died);

    set_entity_controlled(entity);
}

void ECMover::move_entity(const QPointF &to_pos) {
    assert(entity_controlled() != nullptr);
    is_moving_entity_ = true;
    move_entity_(to_pos);
}

void ECMover::stop_moving_entity() {
    is_moving_entity_ = false;
    stop_moving_entity_();
}

bool ECMover::is_moving_entity() { return is_moving_entity_ && entity_controlled() != nullptr; }
