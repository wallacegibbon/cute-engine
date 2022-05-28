#include "InventoryUser.h"
#include "Entity.h"
#include "EntityTargetItem.h"
#include "EquipableItem.h"
#include "Game.h"
#include "Inventory.h"
#include "InventoryViewer.h"
#include "NoTargetItem.h"
#include "PointTargetItem.h"
#include "Slot.h"

using namespace cute;

InventoryUser::InventoryUser(Game *game, Inventory *inventory)
        : inventory_viewer_(new InventoryViewer(inventory)), game_(game) {

    connect(inventory_viewer_.get(), &InventoryViewer::item_clicked, this, &InventoryUser::on_item_clicked);
}

QGraphicsItem *InventoryUser::get_graphics_item() { return inventory_viewer_->get_graphics_item(); }

void InventoryUser::on_item_clicked(Item *item, int button) {
    /// is this nullptr checking necessary ?
    if (item == nullptr) {
        return;
    }

    if (button == Qt::RightButton) {
        drop_item_on_the_ground(item);
        return;
    }

    /// For now, all buttons except the right button are considered "use item"

    NoTargetItem *as_no_target_item = dynamic_cast<NoTargetItem *>(item);
    if (as_no_target_item) {
        use_no_target_item(as_no_target_item);
        return;
    }

    EquipableItem *as_equipable_item = dynamic_cast<EquipableItem *>(item);
    if (as_equipable_item != nullptr) {
        use_equipable_item(as_equipable_item);
        return;
    }

    PointTargetItem *as_point_target_item = dynamic_cast<PointTargetItem *>(item);
    if (as_point_target_item != nullptr) {
        use_point_target_item(as_point_target_item);
        return;
    }

    EntityTargetItem *as_entity_target_item = dynamic_cast<EntityTargetItem *>(item);
    if (as_entity_target_item != nullptr) {
        use_entity_target_item(as_entity_target_item);
        return;
    }
}

void InventoryUser::drop_item_on_the_ground(Item *item) {
    /// if its an equipeable item that is equiped, unequip it
    EquipableItem *as_equipable_item = dynamic_cast<EquipableItem *>(item);
    if (as_equipable_item) {
        if (as_equipable_item->is_equipped()) {
            as_equipable_item->slot_equipped_in()->unequip();
        }
    }
    /// drop item on ground
    item->inventory()->remove_item(item);
}

void InventoryUser::use_no_target_item(NoTargetItem *item) { item->use(); }

/// traverse through all slots of entity looking for a slot that can equip this type of EquipableItem
/// if there is already an item in that slot, swap them
void InventoryUser::use_equipable_item(EquipableItem *item) {
    Inventory *entitys_inventory = item->inventory();
    if (entitys_inventory == nullptr) {
        return;
    }
    Entity *owner = entitys_inventory->owner();
    Slot *viable_slot = nullptr;
    for (Slot *slot : owner->get_slots()) {
        if (slot->can_be_equipped(item)) {
            viable_slot = slot;
            break;
        }
    }
    if (viable_slot) {
        /// if there is already an item in that slot, take it out
        if (viable_slot->is_filled()) {
            viable_slot->unequip();
        }
        viable_slot->equip(item);
    }
}

/// Put mouse_mode of the game to select position, connect to mouse_mode's position_selected
void InventoryUser::use_point_target_item(PointTargetItem *item) {
    game_->set_mouse_mode(Game::MouseMode::SelectPosition);
    last_item_used_ = item;

    disconnect(game_, &Game::position_selected, this,
               &InventoryUser::on_position_selected_while_using_point_target_item);
    disconnect(game_, &Game::select_canceled, this, &InventoryUser::on_position_selection_canceled);

    connect(game_, &Game::position_selected, this, &InventoryUser::on_position_selected_while_using_point_target_item);
    connect(game_, &Game::select_canceled, this, &InventoryUser::on_position_selection_canceled);
}

/// Put mouse_mode of the game to select entity, connect to mouse_mode's entity_selected
void InventoryUser::use_entity_target_item(EntityTargetItem *item) {
    game_->set_mouse_mode(Game::MouseMode::SelectEntity);
    last_item_used_ = item;

    disconnect(game_, &Game::entity_selected, this, &InventoryUser::on_entity_selected_while_using_entity_target_item);
    disconnect(game_, &Game::select_canceled, this, &InventoryUser::on_entity_selection_canceled);

    connect(game_, &Game::entity_selected, this, &InventoryUser::on_entity_selected_while_using_entity_target_item);
    connect(game_, &Game::select_canceled, this, &InventoryUser::on_entity_selection_canceled);
}

void InventoryUser::on_position_selected_while_using_point_target_item(QPointF pos) {
    PointTargetItem *as_point_target_item = dynamic_cast<PointTargetItem *>(last_item_used_);
    assert(as_point_target_item != nullptr);

    as_point_target_item->use(pos);

    disconnect(game_, &Game::position_selected, this,
               &InventoryUser::on_position_selected_while_using_point_target_item);
    disconnect(game_, &Game::select_canceled, this, &InventoryUser::on_position_selection_canceled);

    game_->set_mouse_mode(Game::MouseMode::Regular);
    last_item_used_ = nullptr;
}

void InventoryUser::on_entity_selected_while_using_entity_target_item(Entity *entity) {
    EntityTargetItem *as_entity_target_item = dynamic_cast<EntityTargetItem *>(last_item_used_);
    assert(as_entity_target_item != nullptr);

    as_entity_target_item->use(entity);

    disconnect(game_, &Game::entity_selected, this, &InventoryUser::on_entity_selected_while_using_entity_target_item);
    disconnect(game_, &Game::select_canceled, this, &InventoryUser::on_entity_selection_canceled);

    game_->set_mouse_mode(Game::MouseMode::Regular);
    last_item_used_ = nullptr;
}

void InventoryUser::on_position_selection_canceled() {
    disconnect(game_, &Game::position_selected, this,
               &InventoryUser::on_position_selected_while_using_point_target_item);
    disconnect(game_, &Game::select_canceled, this, &InventoryUser::on_position_selection_canceled);

    game_->set_mouse_mode(Game::MouseMode::Regular);
    last_item_used_ = nullptr;
}

void InventoryUser::on_entity_selection_canceled() {
    disconnect(game_, &Game::entity_selected, this, &InventoryUser::on_entity_selected_while_using_entity_target_item);
    disconnect(game_, &Game::select_canceled, this, &InventoryUser::on_entity_selection_canceled);

    game_->set_mouse_mode(Game::MouseMode::Regular);
    last_item_used_ = nullptr;
}