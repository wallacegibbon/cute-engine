#include "ItemCell.h"
#include "EntitySprite.h"
#include "Inventory.h"
#include "Item.h"
#include "Panel.h"
#include "Sprite.h"

using namespace cute;

ItemCell::ItemCell(int width, int height, Item *item)
        : background_(new Panel()), picture_(new QGraphicsPixmapItem(background_->get_graphics_item())), item_(item) {

    num_charges_text_ = new QGraphicsSimpleTextItem(picture_.get());
    /// set bg width/height
    background_->set_width(width);
    background_->set_height(height);

    /// listen to when the background emits clicked event
    connect(background_.get(), &Panel::clicked, this, &ItemCell::on_background_clicked);

    set_item(item);
}

void ItemCell::set_item(Item *item) {
    /// stop listening to previous item's 'num charges changed' event
    if (item_) {
        disconnect(item_, &Item::num_charges_changed, this, &ItemCell::on_item_charges_changed);
    }
    /// listen to new item's 'num charges changed' event
    item_ = item;
    connect(item_, &Item::num_charges_changed, this, &ItemCell::on_item_charges_changed);

    draw();
}

void ItemCell::set_size(int width, int height) {
    background_->set_width(width);
    background_->set_height(height);
}

void ItemCell::set_background_color(const QColor &color) { background_->set_background_color(color); }

void ItemCell::set_background_pixmap(const QPixmap &pixmap) { background_->set_background_pixmap(pixmap); }

QGraphicsItem *ItemCell::get_graphics_item() { return background_->get_graphics_item(); }

void ItemCell::on_background_clicked(Panel *panel, QPointF pos, int button) { emit clicked(this, button); }

void ItemCell::on_item_charges_changed(Item *sender) {
    num_charges_text_->setText(QString::number(sender->num_of_charges()));
}

void ItemCell::draw() {
    picture_->setPixmap(QPixmap());
    if (!item_) {
        return;
    }
    double width = background_->width();
    double height = background_->height();
    if (item_->icon().isNull()) {
        picture_->setPixmap(item_->sprite()->currently_displayed_frame().scaled(width - 20, height - 20));
    } else {
        picture_->setPixmap(item_->icon().scaled(width - 20, height - 20));
    }
    picture_->setPos(10, 10);

    num_charges_text_->setText(QString::number(item_->num_of_charges()));
    num_charges_text_->setBrush(QBrush(QColor(Qt::white)));
    num_charges_text_->setPos(width - 25, height - 30);
    num_charges_text_->setZValue(picture_->zValue() + 1);
}
