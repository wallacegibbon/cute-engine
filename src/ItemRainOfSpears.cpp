#include "ItemRainOfSpears.h"
#include "RainOfSpearsAbility.h"

using namespace cute;

ItemRainOfSpears::ItemRainOfSpears() {
    set_description("Rains spears in the vicinity. The spears damage enemies.");
    set_ability(new RainOfSpearsAbility());
}

void ItemRainOfSpears::use_() {
    /// do nothing
    /// (when item has an ability (which this one always does), when item is used, it will use ability)
}
