#include "MCRegionEmitter.h"
#include "Entity.h"
#include "Map.h"
#include "stl_helper.h"

using namespace cute;

MCRegionEmitter::MCRegionEmitter(Map *controlled_map, const QPolygonF &region)
        : MapController(controlled_map), region_(region) {
    on_entity_entered_callback_ = [](MCRegionEmitter *, Entity *) {};
    on_entity_left_callback_ = [](MCRegionEmitter *, Entity *) {};

    connect(controlled_map, &Map::entity_moved, this, &MCRegionEmitter::on_entity_moved);
    connect(controlled_map, &Map::entity_added, this, &MCRegionEmitter::on_entity_added);
    connect(controlled_map, &Map::entity_removed, this, &MCRegionEmitter::on_entity_added);
}

MCRegionEmitter::~MCRegionEmitter() { disconnect(controlled_map(), 0, this, 0); }

void MCRegionEmitter::set_on_entity_entered_callback(const std::function<void(MCRegionEmitter *, Entity *)> &callback) {
    on_entity_entered_callback_ = callback;
}

void MCRegionEmitter::set_on_entity_left_callback(const std::function<void(MCRegionEmitter *, Entity *)> &callback) {
    on_entity_left_callback_ = callback;
}

bool MCRegionEmitter::intersects_with_entity(Entity *entity) {
    return entity->bounding_polygon_in_map().intersects(region_);
}

void MCRegionEmitter::on_entity_moved(Map *sender, Entity *entity) {
    if (stl_helper::contains(entered_entities_, entity)) {
        /// case: Entity *just* left region
        if (!intersects_with_entity(entity)) {
            stl_helper::remove(entered_entities_, entity);
            on_entity_left_callback_(this, entity);
            emit entity_left(this, entity);
        }
    } else {
        /// case: Entity *just* entered region
        if (intersects_with_entity(entity)) {
            stl_helper::add(entered_entities_, entity);
            on_entity_entered_callback_(this, entity);
            emit entity_entered(this, entity);
        }
    }
}

void MCRegionEmitter::on_entity_added(Map *sender, Entity *entity) {
    if (intersects_with_entity(entity)) {
        stl_helper::add(entered_entities_, entity);
        on_entity_entered_callback_(this, entity);
        emit entity_entered(this, entity);
    }
}

void MCRegionEmitter::on_entity_removed(Map *sender, Entity *entity) {
    if (stl_helper::contains(entered_entities_, entity)) {
        stl_helper::remove(entered_entities_, entity);
        on_entity_left_callback_(this, entity);
        emit entity_left(this, entity);
    }
}
