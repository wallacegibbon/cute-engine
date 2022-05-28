#include "ECBodyThruster.h"
#include "BodyThrust.h"
#include "ECChaser.h"
#include "QtUtilities.h"

using namespace cute;

ECBodyThruster::ECBodyThruster(Entity *entity) : EntityController(entity) {
    periodic_check_timer_ = new QTimer(this);
    controller_chase_enemies_ = new ECChaser(entity);
    body_thrust_ability_ = new BodyThrust(entity);

    /// set stop distance for chaser to thrust distance of body thrust ability
    controller_chase_enemies_->set_stop_distance(body_thrust_ability_->thrust_distance());

    /// listen to chase controller
    connect(controller_chase_enemies_, &ECChaser::entity_chase_continued, this, &ECBodyThruster::on_chase_continued);
    connect(controller_chase_enemies_, &ECChaser::entity_chase_started, this, &ECBodyThruster::on_chase_continued);
    connect(controller_chase_enemies_, &ECChaser::entity_chase_paused, this, &ECBodyThruster::on_chase_paused);

    connect(periodic_check_timer_, &QTimer::timeout, this, &ECBodyThruster::on_periodic_check);
}

void ECBodyThruster::add_target_entity(Entity *entity) { controller_chase_enemies_->add_chasee(entity); }

void ECBodyThruster::remove_target_entity(Entity *entity) { controller_chase_enemies_->remove_chasee(entity); }

std::unordered_set<Entity *> ECBodyThruster::target_entities() const { return controller_chase_enemies_->chasees(); }

void ECBodyThruster::set_thrust_distance(double distance) {
    controller_chase_enemies_->set_stop_distance(distance);
    body_thrust_ability_->set_thrust_distance(distance);
}

/// Sets the animation to play while the controlled entity is thrusting forward.
/// Pass in "" if you don't want a special animation to be played.
void ECBodyThruster::set_animation_to_play_while_thrusting(const std::string &animation) {
    body_thrust_ability_->set_animation_to_play_while_thrusting(animation);
}

void ECBodyThruster::set_show_FOV(bool tf) { controller_chase_enemies_->set_show_FOV(tf); }

void ECBodyThruster::on_chase_continued(Entity *entity_chased, double distance) {
    last_entity_chased_ = entity_chased;
    periodic_check_timer_->stop();
    body_thrust_if_close_enough();
}

void ECBodyThruster::on_chase_paused(Entity *entity) {
    last_entity_chased_ = entity;
    periodic_check_timer_->start(1000);
    body_thrust_if_close_enough();
}

void ECBodyThruster::on_periodic_check() { body_thrust_if_close_enough(); }

void ECBodyThruster::body_thrust_if_close_enough() {
    if (last_entity_chased_.isNull()) {
        return;
    }
    double dist = QtUtils::distance(last_entity_chased_->pos(), entity_controlled()->pos());
    if (dist < body_thrust_ability_->thrust_distance() && !body_thrust_ability_->thrusting()) {
        body_thrust_ability_->use_implementation();
        emit thrusted(last_entity_chased_);
    }
}
