#include "MCSpawner.h"
#include "AngledSprite.h"
#include "ECBodyThruster.h"
#include "Entity.h"
#include "Map.h"
#include "SpriteSheet.h"

using namespace cute;

MCSpawner::MCSpawner(Map *map, const QRectF &region, int max, double num_per_second, EntityCreator *entity_creator)
        : map_(map), region_(region), entity_creator_(entity_creator), max_(max), num_per_sec_(num_per_second) {

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &MCSpawner::on_timeout);
    turn_on();
}

void MCSpawner::turn_on() { timer_->start(1000 / num_per_sec_); }
void MCSpawner::turn_off() { timer_->stop(); }

void MCSpawner::on_timeout() {
    Entity *entity = entity_creator_->create_entity();

    /// TODO: change the group to be enemy to
    Game::game->diplomacy_manager().set_relationship(entity->group(), 1, Relationship::ENEMY);

    map_->add_entity(entity);
    entity->set_pos(random_.rand_QPointF(region_));
}
