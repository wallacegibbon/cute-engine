#include "ECCameraFollower.h"
#include "Game.h"
#include "Map.h"

using namespace cute;

ECCameraFollower::ECCameraFollower(Entity *entity) : EntityController(entity) {
    assert(entity != nullptr);
    connect(entity, &Entity::moved, this, &ECCameraFollower::on_entity_moved);
}

void ECCameraFollower::on_entity_moved(Entity *entity, QPointF from_pos, QPointF to_pos) {
    /// if the entity is not in a map at this moment, do nothing
    Map *entitys_map = entity->map();
    if (entitys_map == nullptr) {
        return;
    }
    /// if the entitys map is not in a game right now, do nothing
    Game *game = entitys_map->game();
    if (game == nullptr) {
        return;
    }
    game->set_center_cam_pos(to_pos);
}
