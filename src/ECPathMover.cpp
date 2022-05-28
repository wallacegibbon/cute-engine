#include "ECPathMover.h"
#include "AsyncShortestPathFinder.h"
#include "ECRotater.h"
#include "EntitySprite.h"
#include "Map.h"
#include "Sprite.h"
#include "Utilities.h"

using namespace cute;

ECPathMover::ECPathMover(Entity *entity) : ECMover(entity), pf_(new AsyncShortestPathFinder()) {
    move_timer_ = new QTimer(this);
    rotater_ = new ECRotater(entity);
    connect(pf_.get(), &AsyncShortestPathFinder::path_found, this, &ECPathMover::on_path_calculated);
}

/// Executed when the async path finder has succesfully calculated a requested path.
/// Will start the timer to make the entity move on the path.
void ECPathMover::on_path_calculated(std::vector<QPointF> path) {
    /// stop/clear previous movement
    stop_moving_entity();

    /// if entity to move is dead by now, do nothing
    Entity *ent = entity();
    if (ent == nullptr) {
        return;
    }
    /// if entity tried to move from same cell to same cell, do nothing
    if (path.size() == 0 || path.size() == 1) {
        return;
    }
    path.pop_back();

    /// set up variables for new path
    points_to_follow_ = path;
    if (points_to_follow_.size() > 1) {
        /// start following the 1-eth point (0-eth causes initial backward movement)
        target_point_index_ = 1;
    }
    connect(move_timer_, &QTimer::timeout, this, &ECPathMover::on_move_step);
    move_timer_->start(s_to_ms(frequency(step_size_, ent->speed())));

    /// play walk animation (if controlled entity has one)
    EntitySprite *entitys_sprite = ent->sprite();
    if (entitys_sprite != nullptr && entitys_sprite->has_animation("walk")) {
        ent->sprite()->play("walk", -1, 10, 0);
    }
}

void ECPathMover::on_move_step() {
    /// if the entity is destroyed, disconnect
    Entity *ent = entity();
    if (ent == nullptr) {
        stop_moving_entity();
        return;
    }
    /// if the entity is not in a map, do nothing
    Map *entitys_map = ent->map();
    if (entitys_map == nullptr) {
        return;
    }

    /// if there are no more points to follow and entity has reached its target, stop moving
    if (target_point_index_ >= points_to_follow_.size() - 1 && target_point_reached()) {
        QPointF snap_pt = points_to_follow_[target_point_index_];
        ent->set_cell_pos(entitys_map->pathing_map().point_to_cell(snap_pt));
        stop_moving_entity();
        return;
    }

    /// if there are more points to follow and entity has reached its target
    bool has_more_points = target_point_index_ < points_to_follow_.size();
    if (has_more_points && target_point_reached()) {
        QPointF snap_pt = points_to_follow_[target_point_index_];
        ent->set_cell_pos(entitys_map->pathing_map().point_to_cell(snap_pt));
        target_point_index_++;
        if (!always_face_target_position_) {
            rotater_->rotate_towards(points_to_follow_[target_point_index_]);
        }
    }

    step_towards_target();
}

void ECPathMover::move_entity_(const QPointF &to_pos) {
    /// make sure the entiy is in a map
    Map *entitys_map = entity()->map();
    assert(entitys_map != nullptr);

    /// tell async path finder to start finding path to the pos,
    /// when found, the path finder will emit an event (which we listen to)
    pf_->find_path(entitys_map->pathing_map(), entity()->pos(), to_pos);
}

/// This function is executed when the MoveBehavior is asked to stop moving the entity.
void ECPathMover::stop_moving_entity_() {
    move_timer_->disconnect();
    points_to_follow_.clear();
    target_point_index_ = 0;

    /// if controlled entity is dead, do nothing
    Entity *ent = entity();
    if (ent == nullptr) {
        return;
    }

    /// otherwise, play stand animation (if controlled entity has one)
    EntitySprite *entitys_sprite = ent->sprite();
    if (entitys_sprite != nullptr && entitys_sprite->has_animation("stand")) {
        ent->sprite()->play("stand", -1, 10, 0);
    }
}

bool ECPathMover::target_point_reached() {
    /// if there are no target points, return true
    if (points_to_follow_.size() == 0) {
        return true;
    }
    QLineF ln(entity()->pos(), points_to_follow_[target_point_index_]);
    return ln.length() < step_size_;
}

void ECPathMover::step_towards_target() {
    /// do nothing if there are no points to follow
    if (points_to_follow_.size() <= 0) {
        return;
    }

    Entity *ent = entity();

    QLineF ln(ent->pos(), points_to_follow_[target_point_index_]);
    ln.setLength(step_size_);

    double new_x = ent->pos().x() + ln.dx();
    double new_y = ent->pos().y() + ln.dy();
    QPointF new_pt(new_x, new_y);

    if (always_face_target_position_) {
        rotater_->rotate_towards(points_to_follow_.back());
    }

    ent->set_pos(new_pt);
    emit moved(new_pt);
}
