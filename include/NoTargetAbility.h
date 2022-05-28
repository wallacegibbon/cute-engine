#pragma once

#include "Ability.h"
#include "Vendor.h"

namespace cute {

class NoTargetAbility : public Ability {
public:
    NoTargetAbility(Entity *owner, QPixmap icon = QPixmap()) : Ability(owner, icon) {}
    void use();

protected:
    virtual void use_implementation() = 0;
};

} // namespace cute
