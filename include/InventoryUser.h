#pragma once

#include "GUI.h"
#include "InventoryViewer.h"
#include "Vendor.h"

class QGraphicsItem;

namespace cute {

class InventoryViewer;
class Inventory;
class Item;
class Entity;

// A GUI that allows "using" the Items in an Inventory.
class InventoryUser : public GUI {
    Q_OBJECT

public:
    InventoryUser(Game *game, Inventory *inventory = nullptr);
    QGraphicsItem *get_graphics_item() override;

public slots:
    void on_item_clicked(Item *item, int button);
    void on_position_selected_while_using_point_target_item(QPointF pos);
    void on_entity_selected_while_using_entity_target_item(Entity *entity);
    void on_position_selection_canceled();
    void on_entity_selection_canceled();

private:
    void use_no_target_item(NoTargetItem *item);
    void use_equipable_item(EquipableItem *item);
    void use_point_target_item(PointTargetItem *item);
    void use_entity_target_item(EntityTargetItem *item);
    void drop_item_on_the_ground(Item *item);

private:
    std::unique_ptr<InventoryViewer> inventory_viewer_;
    Game *game_;
    Item *last_item_used_ = nullptr;
};

} // namespace cute
