#include "ShopGUI.h"
#include "Button.h"
#include "Game.h"
#include "Inventory.h"
#include "InventoryViewer.h"
#include "Item.h"
#include "Label.h"
#include "Panel.h"
#include "ShopBehaviorNoCost.h"

using namespace cute;

ShopGUI::ShopGUI() : inventory_viewer_(new InventoryViewer()) {
    shop_behavior_ = new ShopBehaviorNoCost();

    panel_ = new Panel();
    panel_->set_parent_GUI(inventory_viewer_.get());
    description_label_ = new Label();
    description_label_->set_parent_GUI(panel_);
    price_label_ = new Label();
    price_label_->set_parent_GUI(panel_);
    buy_button_ = new Button();
    buy_button_->set_parent_GUI(panel_);
    close_button_ = new Button();
    close_button_->set_parent_GUI(panel_);

    inventory_viewer_->set_num_cells_horizontally(5);
    double inventory_viewer_width = inventory_viewer_->width();
    panel_->set_GUI_pos(QPointF(20, inventory_viewer_->height()));
    panel_->set_width(inventory_viewer_width);
    panel_->set_height(150);
    description_label_->set_width(inventory_viewer_width);
    description_label_->set_GUI_pos(QPointF(10, 10));
    price_label_->set_width(inventory_viewer_width);
    price_label_->set_GUI_pos(QPointF(10, 100));
    buy_button_->set_text("buy");
    buy_button_->set_GUI_pos(QPointF(0, panel_->height()));
    close_button_->set_text("close");
    close_button_->set_GUI_pos(QPointF(inventory_viewer_width - close_button_->width(), panel_->height()));

    connect(inventory_viewer_.get(), &InventoryViewer::item_clicked, this, &ShopGUI::on_item_clicked);
    connect(buy_button_, &Button::clicked, this, &ShopGUI::on_buy_button_clicked);
    connect(close_button_, &Button::clicked, this, &ShopGUI::on_close_button_clicked);
}

void ShopGUI::set_seller(Inventory *seller) { inventory_viewer_->set_inventory(seller); }

Inventory *ShopGUI::seller() { return inventory_viewer_->inventory(); }

QGraphicsItem *ShopGUI::get_graphics_item() { return inventory_viewer_->get_graphics_item(); }

void ShopGUI::on_item_clicked(Item *clicked_item, int mouse_button) {
    if (mouse_button != Qt::LeftButton) {
        return;
    }
    selected_item_ = clicked_item;
    /// update description/price
    description_label_->set_text(clicked_item->description());
    price_label_->set_text("price: not implemented yet");
}

void ShopGUI::on_buy_button_clicked() {
    Inventory *b = buyer();
    Inventory *s = seller();

    bool succesfully_bought = shop_behavior_->buy(selected_item_, s, b);
    if (succesfully_bought) {
        s->remove_item(selected_item_);
        b->add_item(selected_item_);
    }
}

void ShopGUI::on_close_button_clicked() {
    Game *g = game();
    if (g) {
        g->remove_GUI(this);
    }
}
