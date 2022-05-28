#pragma once

#include "Map.h"
#include "Node.h"
#include "PointerGrid.h"
#include "Vendor.h"

namespace cute {

class Map;

class MapGrid {
public:
    MapGrid(int columns = 1, int rows = 1) : pointer_grid(columns, rows) {}

    void set_map_at_pos(Map *map, int x, int y) { pointer_grid.set_pointer_at(x, y, map); }
    void set_map_at_pos(Map *map, const Node &pos) { set_map_at_pos(map, pos.x(), pos.y()); }

    Map *map_at(int x, int y) { return pointer_grid.pointer_at(x, y); }
    bool contains(Map *map) const { return pointer_grid.contains(map); }
    Node position_of(Map *map) const { return pointer_grid.position_of(map); }
    std::vector<Map *> maps() const { return pointer_grid.non_null_pointers(); }

private:
    PointerGrid<Map> pointer_grid;
};

} // namespace cute
