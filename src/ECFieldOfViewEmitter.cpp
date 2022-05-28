#include "ECFieldOfViewEmitter.h"
#include "Entity.h"
#include "Map.h"

using namespace cute;

ECFieldOfViewEmitter::ECFieldOfViewEmitter(Entity *entity, double FOV_angle, double FOV_distance)
        : EntityController(entity), field_of_view_angle_(FOV_angle), field_of_view_distance_(FOV_distance) {
    timer_check_FOV_ = new QTimer(this);
    connect(timer_check_FOV_, &QTimer::timeout, this, &ECFieldOfViewEmitter::check_FOV);
    timer_check_FOV_->start(field_of_view_check_delay_ms_);

    /// create polygon item for visualization purposes
    visual_FOV_ = new QGraphicsPolygonItem();
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::red);
    visual_FOV_->setBrush(brush);
    visual_FOV_->setOpacity(0.3);
}

ECFieldOfViewEmitter::~ECFieldOfViewEmitter() {
    /// clean up field of view visualization
    ensure_visual_FOV_removed();
    delete visual_FOV_;
}

/// Executed periodically for the entity controller to check the field of view of the controlled entity.
/// Will emit a signal for each entity that enters/leaves its field of view.
void ECFieldOfViewEmitter::check_FOV() {
    /// approach:
    /// - if the controlled entity is destroyed, stop timer
    /// - if the controlled entity is not in a map at this time, do nothing
    /// - emit entity_entered_FOV signal for any entity that is currently in
    ///   the fov but wasn't earlier
    /// - emit entity_left_FOV signal for any entities that were in the fov
    ///   earlier, but not anymore

    /// if the controlled entity is destroyed, stop
    if (entity_controlled() == nullptr) {
        timer_check_FOV_->disconnect();
        return;
    }

    /// if the controlled entity is not in a map, do nothing
    Map *entitys_map = entity_controlled()->map();
    if (entitys_map == nullptr) {
        return;
    }

    /// emit entity_entered_FOV if any entities just entered the fov
    std::unordered_set<Entity *> entities = entities_in_view();
    for (Entity *entity : entities) {
        /// if were not in fov earlier, emit
        if (entities_in_view_last_time_.count(entity) == 0) {
            entities_in_view_last_time_.insert(entity);
            // qDebug() << "entity entered fov";
            emit entity_entered_FOV(entity);
        }
    }

    /// emit entity_left_FOV if any entities just left the fov
    std::unordered_set<Entity *> copy = entities_in_view_last_time_;
    for (Entity *entity : copy) {
        /// if the entiy is no longer in view, remove from list and emit
        if (entities.count(entity) == 0) {
            entities_in_view_last_time_.erase(entity);
            // qDebug() << "entity left fov";
            emit entity_left_FOV(entity);
        }
    }
}

void ECFieldOfViewEmitter::ensure_visual_FOV_removed() {
    auto entity = entity_controlled();
    if (entity) {
        auto map = entity->map();
        if (map) {
            map->scene()->removeItem(visual_FOV_);
        }
    }
}

std::unordered_set<Entity *> ECFieldOfViewEmitter::entities_in_view() {
    /// - create QPolygon triangel w/ distance and angle
    /// - pass this triangle to map to get entities in their

    Map *entitys_map = entity_controlled()->map();
    assert(entitys_map != nullptr);

    QPointF p1(entity_controlled()->pos());
    QLineF adjacent(p1, QPointF(-5, -5));
    adjacent.setAngle(-1 * entity_controlled()->facing_angle());
    adjacent.setLength(field_of_view_distance_);
    QLineF top_line(adjacent);
    top_line.setAngle(top_line.angle() + field_of_view_angle_ / 2);
    QPointF p2(top_line.p2());
    QLineF bottom_line(adjacent);
    bottom_line.setAngle(bottom_line.angle() - field_of_view_angle_ / 2);
    QPointF p3(bottom_line.p2());

    QVector<QPointF> points;
    points.append(p1);
    points.append(p2);
    points.append(p3);

    QPolygonF poly(points);

    visual_FOV_->setPolygon(poly);

    if (show_FOV_) {
        entitys_map->scene()->removeItem(visual_FOV_);
        entitys_map->scene()->addItem(visual_FOV_);
    }

    std::unordered_set<Entity *> entities = entitys_map->entities(poly);
    entities.erase(entity_controlled());

    return entities;
}

void ECFieldOfViewEmitter::set_check_frequency(double times_per_second) {
    timer_check_FOV_->stop();
    double times_per_ms = times_per_second / 1000.0;
    field_of_view_check_delay_ms_ = 1 / times_per_ms;
    timer_check_FOV_->start(field_of_view_check_delay_ms_);
}

double ECFieldOfViewEmitter::check_frequency() const {
    double times_per_ms = 1 / field_of_view_check_delay_ms_;
    double timers_per_second = times_per_ms * 1000;
    return timers_per_second;
}

void ECFieldOfViewEmitter::turn_on() { timer_check_FOV_->start(field_of_view_check_delay_ms_); }

void ECFieldOfViewEmitter::turn_off() { timer_check_FOV_->stop(); }

void ECFieldOfViewEmitter::set_show_FOV(bool tf) {
    show_FOV_ = tf;
    if (!show_FOV_) {
        ensure_visual_FOV_removed();
    }
}
