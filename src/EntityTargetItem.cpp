#include "EntityTargetItem.h"
#include "EntityTargetAbility.h"
#include "TopDownSprite.h"

using namespace cute;

EntityTargetItem::EntityTargetItem(EntityTargetAbility *ability) : ability_(ability) { set_ability(ability); }

void EntityTargetItem::use(Entity *on_entity) {
    assert(on_entity);

    if (ability_) {
        ability_->set_owner(owner());
        ability_->use(*on_entity);
    } else {
        use_(on_entity);
    }

    /// reduce number of charges
    /// -1 charges represents an infinite number of charges
    if (num_of_charges() != -1) {
        set_num_of_charges(num_of_charges() - 1);
    }
}

/// Giving the item an ability means setting the icon/sprite of the item to the
/// icon of the ability and using the ability when the item is used.
void EntityTargetItem::set_ability(EntityTargetAbility *ability) {
    ability_ = ability;
    if (ability) {
        set_icon(ability->icon());
        set_sprite(new TopDownSprite(ability->icon()));
    }
}
