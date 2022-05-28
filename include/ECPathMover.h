#pragma once

#include "ECMover.h"
#include "Entity.h"
#include "Vendor.h"

class QTimer;

namespace cute {

class AsyncShortestPathFinder;
class ECRotater;

/// A Mover that moves the Entity in a path finding way. The entity will move
/// using a path finding algorithm. You can specifiy if the entity should
/// continue to face its target position or if it should face the direction its
/// heading into.
///
/// Example usage:
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
/// PathMover* pm = new PathMover(entity);
/// pm->set_always_face_target_osition();
/// pm->move_entity(toSomePoint);
/// connect(pm,&PathMover::moved,this,myCallback);
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///
/// @author Abdullah Aghazadah
/// @date 11/28/16

class ECPathMover : public ECMover {
    Q_OBJECT

public:
    ECPathMover(Entity *entity = nullptr);

    /// Sets how many pixels the entity should move every time he moves.
    /// This in effect controlls the "granularity" of the movement.
    /// Higher values means the controlled entity takes bigger steps but infrequently.
    /// Lower values means the controlled entity takes frequent small steps.
    /// Note that this does not effect the speed of the controlled entity, just the movement "granularity"!
    void set_step_size(double step_size) { step_size_ = step_size; }
    double step_size() { return step_size_; }

    bool always_face_target_osition() { return always_face_target_position_; }
    void set_always_face_target_osition(bool tf) { always_face_target_position_ = tf; }

public slots:
    void on_path_calculated(std::vector<QPointF> path);
    void on_move_step();

signals:
    void moved(const QPointF &to_pos);

protected:
    void move_entity_(const QPointF &to_pos) override;
    void stop_moving_entity_() override;

private:
    bool target_point_reached();
    void step_towards_target();

private:
    /// controlled entity should continueously face last pos in path
    /// (instead of facing forward in each segment of path)
    bool always_face_target_position_ = false;

    /// how "granular" the movement should be
    int step_size_ = 5;

    QTimer *move_timer_;
    std::unique_ptr<AsyncShortestPathFinder> pf_;
    ECRotater *rotater_;

    /// moving helper attributes
    std::vector<QPointF> points_to_follow_;
    int target_point_index_ = 0;
    bool currently_moving_ = false;
};

} // namespace cute
