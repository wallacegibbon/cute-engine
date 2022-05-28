#pragma once

#include "Item.h"
#include "Vendor.h"

class QPointF;

namespace cute {

class Entity;
class Slot;

class EquipableItem : public Item {
    /// Slot and EquipableItem reference each other. After an EquipableItem has been equipped in a Slot,
    /// the slot needs to update the slot_ pointer of the EquipableItem.
    friend class Slot;

public:
    QPointF attachment_point() const { return attachment_point_; }
    void set_attachment_point(QPointF point) { attachment_point_ = point; }

    bool is_equipped() const { return slot_equipped_in_ != nullptr; }
    Slot *slot_equipped_in() const { return slot_equipped_in_; }

private:
    Slot *slot_equipped_in_ = nullptr;
    QPointF attachment_point_;
};

} // namespace cute
