#include "ECGUIShower.h"
#include "GUI.h"
#include "Game.h"
#include "Map.h"
#include "QtUtilities.h"
#include "stl_helper.h"

using namespace cute;

ECGUIShower::ECGUIShower(Entity *controlled_entity, GUI *gui) : EntityController(controlled_entity), gui_(gui) {
    connect(controlled_entity, &Entity::moved, this, &ECGUIShower::on_entity_moved);
}

void ECGUIShower::add_entity_of_interest(Entity *entity) {
    disconnect(entity, &Entity::moved, this, &ECGUIShower::on_entity_moved);
    connect(entity, &Entity::moved, this, &ECGUIShower::on_entity_moved);
    entities_of_interest_.insert(QPointer<Entity>(entity));
}

/// Executed when any of the entities (the controlled entity or any of the entities of interest) moves.
/// If the distance between *any* of the entities
/// of interest and the controlled entity is less than the threshold distance,
/// will make sure the dialog gui is shown, otherwise will make sure its hidden.
void ECGUIShower::on_entity_moved() {
    /// algorithmic approach:
    /// - compute the distance between each entity of interest and the controlled entity
    /// - if any of those distances is less than the threshold distance, make
    ///   sure the dialog gui is visible, else, make sure its invisible

    /// implementation appraoch:
    /// - use a range based for each loop to compare the distance of each

    Entity *ec = entity_controlled();

    /// if the controlled entity is dead, do nothing
    /// (this EC will be deleted soon if it hasn't already so no need to disconnect)
    if (ec == nullptr) {
        return;
    }

    /// lets clear dead entities of interst
    for (Entity *e : entities_of_interest_) {
        if (e == nullptr) {
            entities_of_interest_.erase(e);
        }
        /// TODO: solve the mess
        // stl_helper::remove(entities_of_interest_, e);                   /// doesn't work for some reason!
        // stl_helper::remove(entities_of_interest_, QPointer<Entity>(e)); /// have to do this! why!? TODO: potential
    }

    /// set up variables
    double threshold = distance();
    Map *entitys_map = ec->map();
    Game *entitys_game = nullptr;
    if (entitys_map) {
        entitys_game = entitys_map->game();
    }

    /// the *first* entity within range
    Entity *entity_within_range = nullptr;
    for (Entity *e : entities_of_interest_) {
        if (QtUtils::distance(e->pos(), ec->pos()) < threshold && e->map() == entitys_map) {
            entity_within_range = e;
        }
    }

    /// if someone met the threshold (i.e. is in range), make sure gui is added
    /// otherwise, make sure its removed
    if (entity_within_range) {
        if (entitys_game) {
            if (!entitys_game->contains_GUI(gui_)) {
                entitys_game->add_GUI(gui_);
                emit GUI_shown(this, entity_within_range);
            }
        }
    } else {
        if (entitys_game) {
            if (entitys_game->contains_GUI(gui_)) {
                entitys_game->remove_GUI(gui_.data());
                emit GUI_hidden(this);
            }
        }
    }
}

void ECGUIShower::on_controlled_entity_dying(Entity *controlled_entity) {
    Entity *ec = entity_controlled();
    Map *entitys_map = ec->map();
    if (entitys_map) {
        Game *entitys_game = entitys_map->game();
        if (entitys_game) {
            entitys_game->remove_GUI(gui_);
        }
    }
}
