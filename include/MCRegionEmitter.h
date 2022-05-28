#pragma once

#include "MapController.h"
#include "Vendor.h"

namespace cute {

class Entity;

class MCRegionEmitter : public MapController {
    Q_OBJECT

public:
    MCRegionEmitter(Map *controlled_map, const QPolygonF &region);
    virtual ~MCRegionEmitter();

    void set_on_entity_entered_callback(const std::function<void(MCRegionEmitter *, Entity *)> &callback);
    void set_on_entity_left_callback(const std::function<void(MCRegionEmitter *, Entity *)> &callback);

signals:
    void entity_entered(MCRegionEmitter *sender, Entity *entity);
    void entity_left(MCRegionEmitter *sender, Entity *entity);

private:
    void on_entity_moved(Map *sender, Entity *entity);
    void on_entity_added(Map *sender, Entity *entity);
    void on_entity_removed(Map *sender, Entity *entity);

    bool intersects_with_entity(Entity *entity);

private:
    QPolygonF region_;
    std::set<Entity *> entered_entities_;
    std::function<void(MCRegionEmitter *, Entity *)> on_entity_entered_callback_;
    std::function<void(MCRegionEmitter *, Entity *)> on_entity_left_callback_;
};

} // namespace cute
