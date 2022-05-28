#pragma once

#include "ECFieldOfViewEmitter.h"
#include "Entity.h"
#include "EntityController.h"
#include "Vendor.h"

class QTimer;

namespace cute {

class ECPathMover;
class ECFieldOfViewEmitter;

/// An entity controller that makes it so the controlled entity will chase certain other entities
/// that enter its field of view.
///
/// Use add_chasee() to add entities that should be chased when within the field of view.
///
/// Whenever the controlled entity *starts* chasing an entity, a signal (entity_chase_started()) will be emitted.
/// Everytime the controlled entity takes a step closer to the chased entity
/// another signal (entity_chase_continued()) will be emitted.
///
/// You can set how close the controlled entity will get to the chased entity before it pauses ("stop distance").
/// When the controlled entity gets to within the specified distance
/// a signal (entity_chase_paused()) will be emitted and the controlled entity won't get any closer.
/// When the chased entity gets out of the stop distance,
/// the controlled entity will start chasing again and entity_chase_continued() will be emitted.
///
/// Some relavent terminlogy used in the context of ECChaser:
/// - a "chasee" is any Entity that will be chased if it enters the field of view of the controlled entity.
/// - the "target" or "target chasee" entity is the chasee that the controller is *currently* chasing.

class ECChaser : public EntityController {
    Q_OBJECT

public:
    ECChaser(Entity *entity);

    void add_chasee(Entity *entity);
    void remove_chasee(Entity *entity);
    std::unordered_set<Entity *> chasees() const { return chasees_; }

    void stop_chasing();
    void start_chasing();

    void set_stop_distance(double distance) { stop_distance_ = distance; }
    double stop_distance() const { return stop_distance_; }

    void set_show_FOV(bool tf);

signals:
    void entity_chase_started(Entity *chased_entity, double dist_to_chased_entity);
    void entity_chase_continued(Entity *chased_entity, double dist_to_chased_entity);
    void entity_chase_paused(Entity *chased_entity);

public slots:
    void on_entity_enter_FOV(Entity *entity);
    void on_entity_leave_FOV(Entity *entity);

    void on_controlled_entity_move();

    void on_stop_range_enter(Entity *watched, Entity *watching, double range);
    void on_stop_range_leave(Entity *watched, Entity *watching, double range);

    void on_controlled_entity_die(QObject *entity);
    void on_chased_entity_die(QObject *entity);

    void on_controlled_entity_leave_map(Entity *entity);
    void on_chased_entity_leave_map(Entity *entity);

    void on_chasee_destroyed(QObject *chasee);

    void on_controlled_entity_enter_map();

    void chase_step();

private:
    void connect_to_target_signals();
    void disconnect_from_target_signals();

private:
    std::unordered_set<Entity *> chasees_;

    double stop_distance_ = 100;

    /// helper controller that emits events whenever
    /// another entity enters/leaves the controlled entity's FOV (field of view)
    ECFieldOfViewEmitter *FOV_emitter_;

    ECPathMover *path_mover_;
    QTimer *chase_timer_;

    /// controlled entity is w/i stop distance of chased entity
    bool paused_ = false;

    QPointer<Entity> target_entity_ = nullptr;
};

} // namespace cute
