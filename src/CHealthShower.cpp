#include "CHealthShower.h"
#include "Bar.h"
#include "Entity.h"
#include "EntitySprite.h"
#include "Map.h"
#include "stl_helper.h"

using namespace cute;

void CHealthShower::add_entity(Entity *entity) {
    if (stl_helper::contains(entities_, entity)) {
        return;
    }
    connect(entity, &Entity::health_changed, this, &CHealthShower::on_entity_health_changed);
    connect(entity, &Entity::moved, this, &CHealthShower::on_entity_moved);
    connect(entity, &Entity::map_entered, this, &CHealthShower::on_entity_enters_map);
    connect(entity, &Entity::map_left, this, &CHealthShower::on_entity_leaves_map);

    Map *entitys_map = entity->map();
    if (entitys_map) {
        connect(entitys_map, &Map::set_as_current_map, this, &CHealthShower::on_entity_map_visualized_);
        connect(entitys_map, &Map ::unset_as_current_map, this, &CHealthShower::on_entity_map_unvisualized);
    }

    connect(entity, &Entity::dying, this, &CHealthShower::on_entity_dying);

    Bar *bar = new Bar();
    bar->set_width(40);
    bar->set_min_value(0);
    bar->set_max_value(entity->max_health());
    bar->set_current_value(entity->health());
    entity_to_bar_[entity] = bar;

    Game *entitys_game = nullptr;
    if (entitys_map) {
        entitys_game = entitys_map->game();
    }
    if (entitys_game) {
        connect(entitys_game, &Game::cam_moved, this, &CHealthShower::update_pos_of_all_bars);
        entitys_game->add_GUI(bar);
        set_bar_pos(entity, bar);
    }
    stl_helper::add(entities_, entity);
}

void CHealthShower::on_entity_moved(Entity *sender, QPointF from_pos, QPointF to_pos) {
    Map *entitys_map = sender->map();
    Game *entitys_game = nullptr;
    if (entitys_map) {
        entitys_game = entitys_map->game();
    }

    Bar *bar = entity_to_bar_[sender];
    if (entitys_game) {
        set_bar_pos(sender, bar);
    }
}

void CHealthShower::on_entity_health_changed(Entity *sender) {
    Bar *bar = entity_to_bar_[sender];
    bar->set_min_value(0);
    bar->set_max_value(sender->max_health());
    bar->set_current_value(sender->health());
}

/// Executed when one of the entitys of the CHealthShower enters a map.
/// If the entered map is visualized, will make sure its health bar is added to it, other wise
/// will make sure the health bar is removed.
void CHealthShower::on_entity_enters_map(Entity *sender, Map *map_entered, Map *old_map) {

    disconnect(map_entered, &Map::set_as_current_map, this, &CHealthShower::on_entity_map_visualized_);
    disconnect(map_entered, &Map ::unset_as_current_map, this, &CHealthShower::on_entity_map_unvisualized);
    connect(map_entered, &Map::set_as_current_map, this, &CHealthShower::on_entity_map_visualized_);
    connect(map_entered, &Map ::unset_as_current_map, this, &CHealthShower::on_entity_map_unvisualized);

    Game *maps_game = map_entered->game();
    Bar *bar = entity_to_bar_[sender];
    /// if the new map is visualized, add gui, other wise, remove it
    if (maps_game) {
        maps_game->add_GUI(bar);
        set_bar_pos(sender, bar);
    } else {
        maps_game->remove_GUI(bar);
    }
}

void CHealthShower::on_entity_leaves_map(Entity *sender, Map *map_left) {
    Game *maps_game = map_left->game();
    if (maps_game) {
        maps_game->remove_GUI(entity_to_bar_[sender]);
    }
}

/// Executed when the map of an entity is visualized.
/// Will make sure its health bar is added to the game.
void CHealthShower::on_entity_map_visualized_(Map *map_visualized) {
    Game *game = map_visualized->game();
    std::vector<Entity *> entities_in_map;
    for (Entity *entity : entities_) {
        if (entity->map() == map_visualized) {
            entities_in_map.push_back(entity);
        }
    }
    for (Entity *entity : entities_in_map) {
        Bar *bar = entity_to_bar_[entity];
        game->add_GUI(bar);
        set_bar_pos(entity, bar);
    }
}

void CHealthShower::on_entity_map_unvisualized(Map *map_un_visualized) {
    Game *game = nullptr;
    std::vector<Entity *> entities_in_map;
    for (Entity *entity : entities_) {
        Map *entitys_map = entity->map();
        if (entitys_map) {
            if (entitys_map == map_un_visualized) {
                entities_in_map.push_back(entity);
            }
            if (entitys_map->game() != nullptr) {
                game = entitys_map->game();
            }
        }
    }
    for (Entity *entity : entities_in_map) {
        Bar *bar = entity_to_bar_[entity];
        game->remove_GUI(bar);
    }
}

void CHealthShower::on_entity_dying(Entity *sender) {
    disconnect(sender, &Entity::health_changed, this, &CHealthShower::on_entity_health_changed);
    disconnect(sender, &Entity::moved, this, &CHealthShower::on_entity_moved);
    disconnect(sender, &Entity::map_entered, this, &CHealthShower::on_entity_enters_map);
    disconnect(sender, &Entity::map_left, this, &CHealthShower::on_entity_leaves_map);

    Map *map = sender->map();
    if (map) {
        Game *game = map->game();
        if (game) {
            game->remove_GUI(entity_to_bar_[sender]);
        }
    }

    stl_helper::remove(entities_, sender);
    stl_helper::remove(entity_to_bar_, sender);
}

void CHealthShower::update_pos_of_all_bars() {
    for (auto entity_bar_pair : entity_to_bar_) {
        set_bar_pos(entity_bar_pair.first, entity_bar_pair.second);
    }
}

void CHealthShower::set_bar_pos(Entity *entity, Bar *bar) {
    if (entity->map() == nullptr) {
        return;
    }
    Game *game = entity->map()->game();
    if (game == nullptr) {
        return;
    }
    double x = entity->pos().x() - bar->get_GUI_bounding_box().width() / 2;
    double y = entity->top_left().y();
    bar->set_GUI_pos(game->map_from_map(QPointF(x, y)));
}