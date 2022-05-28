#pragma once

#include "Vendor.h"

namespace cute {

class Entity;

class CItemDropper : public QObject {
    Q_OBJECT
public:
    void add_entity(Entity *entity);
    void remove_entity(Entity *entity);

signals:

public slots:
    void on_entity_dies(Entity *entity);

private:
    std::unordered_set<Entity *> entities_;
};

} // namespace cute
