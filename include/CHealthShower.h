#pragma once

#include "Vendor.h"

class QTimer;

namespace cute {

class Entity;
class Map;
class Bar;

class CHealthShower : public QObject {
    Q_OBJECT

public:
    void add_entity(Entity *entity);

public slots:
    void on_entity_moved(Entity *sender, QPointF from_pos, QPointF to_pos);
    void on_entity_health_changed(Entity *sender);
    void on_entity_enters_map(Entity *sender, Map *map_entered, Map *old_map);
    void on_entity_leaves_map(Entity *sender, Map *map_left);
    void on_entity_map_visualized_(Map *map_visualized);
    void on_entity_map_unvisualized(Map *map_un_visualized);
    void on_entity_dying(Entity *sender);
    void update_pos_of_all_bars();

private:
    void set_bar_pos(Entity *entity, Bar *bar);

private:
    std::unordered_set<Entity *> entities_;
    std::unordered_map<Entity *, Bar *> entity_to_bar_;
};

} // namespace cute
