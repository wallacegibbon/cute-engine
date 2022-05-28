#pragma once

#include "Entity.h"
#include "Vendor.h"

namespace cute {

class Inventory;

class Item : public Entity {
    Q_OBJECT

    /// When an Item is removed from the Inventory, the Inventory needs to
    /// set the private inventory_ pointer to nullptr
    friend class Inventory;

public:
    void set_inventory(Inventory *inv);
    Inventory *inventory() { return inventory_; }

    Entity *owner();

    /// Returns the number of charges the Item has. -1 means infinite.
    int num_of_charges() { return num_of_charges_; }

    void set_num_of_charges(int n);

    /// Sets the description text of the Item.
    /// The description text is shown in places like shops, etc...
    void set_description(std::string description) { description_ = description; }
    std::string description() { return description_; }

    /// Sets the icon of the item.
    /// If the item has no icon, guis will use its sprite.
    void set_icon(const QPixmap &pixmap) { icon_ = pixmap; }
    QPixmap icon() { return icon_; }

signals:
    /// Emitted when the number of charges of the items changes (either increases or decreases).
    void num_charges_changed(Item *sender);

protected:
    /// constructor protected so that objects cannot be instantiated, but subclasses
    /// can still use the constructor to set up the base class portion of the state
    Item();

private:
    /// the Inventory, nullptr means on ground
    Inventory *inventory_ = nullptr;

    std::string description_;
    int num_of_charges_ = 1;

    /// optional, guis will use sprite if no icon specified
    QPixmap icon_;
};

} // namespace cute
