#include "ItemGold.h"
#include "Sprite.h"
#include "TopDownSprite.h"

using namespace cute;

ItemGold::ItemGold() {
    TopDownSprite *sprite = new TopDownSprite(QPixmap(":/cute-engine-builtin/resources/graphics/misc/gold.png"));
    set_sprite(sprite);
    set_description("A small amount of gold. Can be used as currency.");
}

void ItemGold::use_() {}
