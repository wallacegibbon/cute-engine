#pragma once

#include "EntityTargetItem.h"
#include "Vendor.h"

namespace cute {

class Entity;

class ItemPushback : public EntityTargetItem {
public:
    ItemPushback();
    void use_(Entity *on_entity) override;
};

} // namespace cute
