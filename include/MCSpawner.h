#pragma once

#include "EntityCreator.h"
#include "RandomGenerator.h"
#include "Vendor.h"

class QTimer;

namespace cute {

class Map;

/// A map controller that will spawn entities randomly within some region of a map.
/// The EntityCreator of the MCSpawner specifies what entity will be created.
/// When it is time to create an entity, MCSpawner will get an entity from its EntityCreator,
/// and then place that entity in a random spot in the map.
class MCSpawner : public QObject {
    Q_OBJECT

public:
    MCSpawner(Map *map, const QRectF &region, int max, double ratePerSec, EntityCreator *entity_creator);

    void turn_on();
    void turn_off();

public slots:
    void on_timeout();

private:
    /// The map that the MCSpawner will spawn Entities in.
    Map *map_;

    /// The region in the map that entities will spawn in.
    QRectF region_;

    /// The maximum number of Entities that the MCSpawner will create.
    /// When some of the Entities die, the MCSpawner will continue to produce new ones.
    int max_;

    /// number of Entities that the MCSpawner will create per second (until max is reached).
    double num_per_sec_;

    EntityCreator *entity_creator_;
    QTimer *timer_;
    RandomGenerator random_;
};

} // namespace cute
