#include "AbilityCell.h"
#include "Ability.h"
#include "Panel.h"
#include "Sprite.h"

using namespace cute;

AbilityCell::AbilityCell(int width, int height, Ability *ability)
        : picture_(new QGraphicsPixmapItem()), background_(new Panel()), ability_(ability) {
    picture_->setParentItem(background_->get_graphics_item());
    background_->set_width(width);
    background_->set_height(height);
    connect(background_.get(), &Panel::clicked, this, &AbilityCell::on_clicked);
    draw();
}

void AbilityCell::set_ability(Ability *ability) {
    ability_ = ability;
    draw();
}

void AbilityCell::set_size(int width, int height) {
    background_->set_width(width);
    background_->set_height(height);
}

void AbilityCell::draw() {
    double width = background_->width();
    double height = background_->height();
    picture_->setPixmap(QPixmap(":/cute-engine-builtin/resources/graphics/misc/default_entity.png")
                                .scaled(width - 20, height - 20));
    picture_->setPos(10, 10);

    /// overwrite with ability's icon if has ability
    if (ability_ != nullptr) {
        picture_->setPixmap(ability_->icon().scaled(width - 20, height - 20));
    }
}
