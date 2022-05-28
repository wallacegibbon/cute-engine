#include "EntityController.h"

using namespace cute;

EntityController::EntityController(Entity *entity) : entity_(entity) {
    /// Entity maintains a collection of EntityControllers that are operating on it.
    /// When an Entity dies, it kills all of its controllers.
    setParent(entity);
    connect(entity, &Entity::dying, this, &EntityController::on_controlled_entity_dying_);
}

void EntityController::set_entity_controlled(Entity *entity) {
    disconnect(entity_, &Entity::dying, this, &EntityController::on_controlled_entity_dying_);
    setParent(entity);
    entity_ = entity;
    connect(entity_, &Entity::dying, this, &EntityController::on_controlled_entity_dying_);
}
