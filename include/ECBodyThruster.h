#pragma once

#include "Entity.h"
#include "EntityController.h"
#include "Vendor.h"

class QTimer;

namespace cute {

class ECChaser;
class BodyThrust;

/// An entity controller that causes the controlled entity to chase certain other entites in its field of view
/// and use the BodyThrust ability on them when close enough.
///
/// Will chase both "target" entities and any entities that are enemies of the controlled entity.

class ECBodyThruster : public EntityController {
    Q_OBJECT

public:
    ECBodyThruster(Entity *entity);

    void add_target_entity(Entity *entity);
    void remove_target_entity(Entity *entity);
    std::unordered_set<Entity *> target_entities() const;

    void set_thrust_distance(double distance);
    void set_animation_to_play_while_thrusting(const std::string &animation);

    void set_show_FOV(bool tf);

public slots:
    void on_chase_continued(Entity *entity_chased, double distance);
    void on_chase_paused(Entity *entity);
    void on_periodic_check();

signals:
    /// Emitted each time the controlled entity thrusts towards an enemy.
    void thrusted(Entity *towardsEnemy);

private:
    void body_thrust_if_close_enough();

private:
    ECChaser *controller_chase_enemies_;
    BodyThrust *body_thrust_ability_;
    QPointer<Entity> last_entity_chased_ = nullptr;
    QTimer *periodic_check_timer_;
};

} // namespace cute
