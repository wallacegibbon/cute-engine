#pragma once

#include <QObject>

namespace cute {

class Map;

class MapController : public QObject {
public:
    MapController(Map *controlled_map) : controlled_map_(controlled_map) {}
    Map *controlled_map() { return controlled_map_; }

private:
    Map *controlled_map_;
};

} // namespace cute
