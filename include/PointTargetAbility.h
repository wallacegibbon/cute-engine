#pragma once

#include "Ability.h"

class QPointF;

namespace cute {

class PointTargetAbility : public Ability {
public:
    PointTargetAbility(Entity *owner = nullptr, QPixmap icon = QPixmap()) : Ability(owner, icon) {}
    void use(const QPointF &at_point);

protected:
    virtual void use_implementation(const QPointF &at_point) = 0;
};

} // namespace cute
