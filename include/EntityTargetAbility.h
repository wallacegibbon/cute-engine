#pragma once

#include "Ability.h"

namespace cute {

class Entity;

class EntityTargetAbility : public Ability {
public:
    EntityTargetAbility(Entity *owner = nullptr, QPixmap icon = QPixmap()) : Ability(owner, icon) {}
    void use(Entity &on_entity);

protected:
    virtual void use_implementation(Entity &on_entity) = 0;
};

} // namespace cute
