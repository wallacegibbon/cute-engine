#pragma once

#include "Vendor.h"

namespace cute {

class Item;
class Entity;
class EquipableItem;
class NoTargetItem;
class EntityTargetItem;
class PointTargetItem;

class Inventory : public QObject {
    Q_OBJECT

public:
    Inventory(Entity *owner = nullptr) : owner_(owner) {}
    ~Inventory();

    /// Returns the Entity that owns this Inventory. Returns nullptr if no Entity owns this inventory.
    Entity *owner() { return owner_; }

    void add_item(Item *item);
    void remove_item(Item *item);
    bool contains(Item *item);

    std::unordered_set<EquipableItem *> get_equipable_items();
    std::unordered_set<NoTargetItem *> get_no_target_items();
    std::unordered_set<EntityTargetItem *> get_entity_target_items();
    std::unordered_set<PointTargetItem *> get_point_target_items();
    std::unordered_set<Item *> get_items() { return items_; }

signals:
    void item_added(Item *item);
    void item_removed(Item *item);

public slots:
    void on_item_dying(Entity *dying_item);

private:
    template <typename T>
    std::unordered_set<T *> get_certain_items();

private:
    Entity *owner_ = nullptr;
    std::unordered_set<Item *> items_;
};

} // namespace cute
