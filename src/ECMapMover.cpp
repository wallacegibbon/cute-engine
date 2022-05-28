#include "ECMapMover.h"
#include "Game.h"
#include "Map.h"
#include "MapGrid.h"
#include "Node.h"

using namespace cute;

ECMapMover::ECMapMover(Entity *entity) : EntityController(entity) {
    assert(entity != nullptr);
    connect(entity, &Entity::moved, this, &ECMapMover::on_entity_moved);
    connect(entity, &Entity::want_to_move_but_cannot, this, &ECMapMover::on_entity_moved);
}

void ECMapMover::on_entity_moved(Entity *controlled_entity, QPointF from_pos, QPointF to_pos) {
    /// do nothing if controlled entity is not in a map
    Entity *entity = entity_controlled();
    Map *entitys_map = entity->map();
    if (entitys_map == nullptr) {
        return;
    }
    /// do nothing if entity's map is not in a map grid
    Game *entitys_game = entitys_map->game();
    if (entitys_game == nullptr) {
        return;
    }
    MapGrid *map_grid = entitys_game->map_grid();
    if (map_grid == nullptr || !map_grid->contains(entitys_map)) {
        return;
    }

    Node current_map_pos = map_grid->position_of(entitys_map);
    QPointF entity_pos = entity->pos();

    if (entity_pos.y() < border_threshold_) {
        Map *entering_map = map_grid->map_at(current_map_pos.x(), current_map_pos.y() - 1);
        if (entering_map) {
            double frac_along = entity->x() / entitys_map->width();
            entering_map->add_entity(entity);
            entity->set_pos(
                    QPointF(frac_along * entering_map->width(), entering_map->height() - border_threshold_ * 2));
        }
    } else if (entity_pos.y() > entitys_map->height() - border_threshold_) {
        Map *entering_map = map_grid->map_at(current_map_pos.x(), current_map_pos.y() + 1);
        if (entering_map) {
            double frac_along = entity->x() / entitys_map->width();
            entering_map->add_entity(entity);
            entity->set_pos(QPointF(frac_along * entering_map->width(), border_threshold_ * 2));
        }
    }

    if (entity_pos.x() < border_threshold_) {
        Map *entering_map = map_grid->map_at(current_map_pos.x() - 1, current_map_pos.y());
        if (entering_map) {
            double frac_along = entity->y() / entitys_map->height();
            entering_map->add_entity(entity);
            entity->set_pos(
                    QPointF(entering_map->width() - border_threshold_ * 2, frac_along * entering_map->height()));
        }
    } else if (entity_pos.x() > entitys_map->width() - border_threshold_) {
        Map *entering_map = map_grid->map_at(current_map_pos.x() + 1, current_map_pos.y());
        if (entering_map) {
            double frac_along = entity->y() / entitys_map->height();
            entering_map->add_entity(entity);
            entity->set_pos(QPointF(border_threshold_ * 2, frac_along * entering_map->height()));
        }
    }
}
