#pragma once

#include "NoTargetItem.h"
#include "Vendor.h"

namespace cute {

class ItemHealthPotion : public NoTargetItem {
public:
    ItemHealthPotion(int amount_to_heal_by_);
    void use_() override;

private:
    int amount_to_heal_by_;
};

} // namespace cute
