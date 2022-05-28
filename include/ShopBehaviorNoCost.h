#pragma once

#include "ShopGUIBehavior.h"

namespace cute {

/// Allows free buying at Shop.
class ShopBehaviorNoCost : public ShopGUIBehavior {
public:
    bool buy(Item *item, Inventory *buyer, Inventory *seller) override;
};

} // namespace cute
