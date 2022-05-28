#pragma once

#include "GUI.h"
#include "InventoryViewer.h"
#include "Vendor.h"

class QGraphicsItem;

namespace cute {

class Inventory;
class Panel;
class Label;
class Button;
class Item;
class ShopGUIBehavior;

/// The seller Inventory will be displayed. Clicking on items results in the item's
/// description being displayed. Clicking on "buy" will result in the selected
/// item being transfered from the seller to the buyer, if the behavior approves.
class ShopGUI : public GUI {
    Q_OBJECT

public:
    ShopGUI();

    void set_buyer(Inventory *buyer) { buyer_ = buyer; }
    Inventory *buyer() { return buyer_; }
    void set_seller(Inventory *seller);
    Inventory *seller();

    QGraphicsItem *get_graphics_item() override;

public slots:
    void on_item_clicked(Item *clicked_item, int mouse_button);
    void on_buy_button_clicked();
    void on_close_button_clicked();

private:
    Inventory *buyer_;
    Item *selected_item_ = nullptr;

    /// the rules of purchasing
    ShopGUIBehavior *shop_behavior_;

    std::unique_ptr<InventoryViewer> inventory_viewer_;

    /// houses the description_label_ and price_label_
    Panel *panel_;

    Label *description_label_;
    Label *price_label_;
    Button *buy_button_;
    Button *close_button_;
};

} // namespace cute
