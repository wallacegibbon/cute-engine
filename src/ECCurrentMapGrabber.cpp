#include "ECCurrentMapGrabber.h"
#include "Game.h"
#include "Map.h"

using namespace cute;

ECCurrentMapGrabber::ECCurrentMapGrabber(Entity *entity) : EntityController(entity) {
    assert(entity != nullptr);
    connect(entity, &Entity::map_entered, this, &ECCurrentMapGrabber::on_entity_enters_map);
}

void ECCurrentMapGrabber::on_entity_enters_map(Entity *controlled_entity, Map *map_entered, Map *map_old) {
    /// do nothing if entity left a map
    if (map_entered == nullptr) {
        return;
    }
    Game::game->set_current_map(map_entered);
}
