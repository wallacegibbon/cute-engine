#include "ItemTeleport.h"
#include "Entity.h"
#include "Inventory.h"
#include "Map.h"
#include "Sound.h"
#include "Sprite.h"
#include "TopDownSprite.h"

using namespace cute;

ItemTeleport::ItemTeleport() {
    TopDownSprite *sprite = new TopDownSprite(QPixmap(":/cute-engine-builtin/resources/graphics/misc/teleport.png"));
    set_sprite(sprite);
    set_description("Allows user to teleport to a specified location within some distance.");
    sound_effect_ = new Sound("qrc:/cute-engine-builtin/resources/sounds/blink.wav", this);
}

void ItemTeleport::use_(const QPointF &point) {
    /// make sure the item is in an inventory
    assert(inventory() != nullptr);

    /// make sure the inventory has an owner
    Entity *owner = inventory()->owner();
    assert(owner != nullptr);

    /// play sound effect
    sound_effect_->play(1);

    /// move owner to the specified location
    owner->set_pos(point);

    /// play a "poof" animation at the location of the owner
    Sprite *poof_sprite = new Sprite();
    poof_sprite->add_frames(":/cute-engine-builtin/resources/graphics/effects/poof", 10, "poof", "poof");
    owner->map()->play_once(poof_sprite, "poof", 50, owner->top_left());
}
