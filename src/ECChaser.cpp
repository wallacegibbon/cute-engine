#include "ECChaser.h"
#include "ECFieldOfViewEmitter.h"
#include "ECPathMover.h"
#include "Game.h"
#include "Map.h"
#include "QtUtilities.h"

using namespace cute;

ECChaser::ECChaser(Entity *entity) : EntityController(entity) {
    chase_timer_ = new QTimer(this);
    FOV_emitter_ = new ECFieldOfViewEmitter(entity);
    path_mover_ = new ECPathMover(entity);

    Map *entitys_map = entity->map();
    if (entitys_map && entitys_map->game() != nullptr) {
        connect(Game::game, &Game::watched_entity_enters_range, this, &ECChaser::on_stop_range_enter);
        connect(Game::game, &Game::watched_entity_leaves_range, this, &ECChaser::on_stop_range_leave);
    }

    connect(entity, &Entity::map_entered, this, &ECChaser::on_controlled_entity_enter_map);

    connect(FOV_emitter_, &ECFieldOfViewEmitter::entity_entered_FOV, this, &ECChaser::on_entity_enter_FOV);
    connect(FOV_emitter_, &ECFieldOfViewEmitter::entity_left_FOV, this, &ECChaser::on_entity_leave_FOV);

    connect(path_mover_, &ECPathMover::moved, this, &ECChaser::on_controlled_entity_move);

    connect(entity_controlled(), &QObject::destroyed, this, &ECChaser::on_controlled_entity_die);

    connect(entity_controlled(), &Entity::map_left, this, &ECChaser::on_controlled_entity_leave_map);

    connect(chase_timer_, &QTimer::timeout, this, &ECChaser::chase_step);

    path_mover_->set_always_face_target_osition(true);
    path_mover_->set_entity(entity);
}

void ECChaser::add_chasee(Entity *entity) {
    chasees_.insert(entity);
    connect(entity, &QObject::destroyed, this, &ECChaser::on_chasee_destroyed);
}

void ECChaser::remove_chasee(Entity *entity) {
    chasees_.erase(entity);
    disconnect(entity, &QObject::destroyed, this, &ECChaser::on_chasee_destroyed);
}

void ECChaser::stop_chasing() {
    /// if currently chasing stop
    if (target_entity_ != nullptr) {
        target_entity_ = nullptr;
        /// stop moving
        chase_timer_->stop();
    }
    FOV_emitter_->turn_off();
}

void ECChaser::start_chasing() { FOV_emitter_->turn_on(); }

void ECChaser::set_show_FOV(bool tf) { FOV_emitter_->set_show_FOV(tf); }

void ECChaser::on_entity_enter_FOV(Entity *entity) {
    /// if the controlled entity already has a target entity, do nothing
    if (target_entity_ != nullptr) {
        return;
    }

    /// if the entering entity is not a chasee, do nothing
    if (chasees_.find(entity) == chasees_.end() &&
        Game::game->diplomacy_manager().get_relationship(entity_controlled()->group(), entity->group()) !=
                Relationship::ENEMY) {
        return;
    }

    /// set entering entity as target entity and connect to some signals from it
    target_entity_ = entity;
    connect_to_target_signals();

    chase_step();

    /// TODO: store timer period in a (modifiable) variable somewhere
    chase_timer_->start(2000);

    double dist = QtUtils::distance(entity_controlled()->pos(), entity->pos());
    emit entity_chase_started(entity, dist);
}

void ECChaser::on_entity_leave_FOV(Entity *entity) {
    /// if leaving entity isn't the target of controlled entity, do nothing
    if (entity != target_entity_) {
        return;
    }

    /// unset as target
    disconnect_from_target_signals();
    target_entity_ = nullptr;

    /// stop moving
    chase_timer_->stop();

    /// if there is another enemy in view, target that one
    std::unordered_set<Entity *> other_entities_in_view = FOV_emitter_->entities_in_view();
    for (Entity *possible_enemy : other_entities_in_view) {
        if (chasees_.find(possible_enemy) != chasees_.end()) {
            on_entity_enter_FOV(possible_enemy);
            return;
        }
    }
}

void ECChaser::on_controlled_entity_move() {
    /// do nothing if nothing being chased
    if (target_entity_.isNull()) {
        return;
    }
    if (!paused_) {
        double dist = QtUtils::distance(entity_controlled()->pos(), target_entity_->pos());
        emit entity_chase_continued(target_entity_, dist);
    }
}

void ECChaser::on_stop_range_enter(Entity *watched, Entity *watching, double range) {
    path_mover_->stop_moving_entity();
    paused_ = true;
    emit entity_chase_paused(target_entity_);
}

void ECChaser::on_stop_range_leave(Entity *watched, Entity *watching, double range) { paused_ = false; }
void ECChaser::on_controlled_entity_die(QObject *entity) { stop_chasing(); }
void ECChaser::on_chased_entity_die(QObject *entity) { stop_chasing(); }

void ECChaser::on_controlled_entity_leave_map(Entity *entity) {
    if (entity->map() == nullptr) {
        stop_chasing();
    }
}

void ECChaser::on_chased_entity_leave_map(Entity *entity) {
    if (entity->map() == nullptr) {
        stop_chasing();
    }
}

void ECChaser::on_chasee_destroyed(QObject *chasee) { chasees_.erase(dynamic_cast<Entity *>(chasee)); }

void ECChaser::on_controlled_entity_enter_map() {
    Game *game = entity_controlled()->map()->game();
    if (game) {
        // disconnect(0, &Game::watched_entity_enters_range, this, &ECChaser::on_stop_range_enter);
        // disconnect(0, &Game::watched_entity_leaves_range, this, &ECChaser::on_stop_range_leave);
        connect(game, &Game::watched_entity_enters_range, this, &ECChaser::on_stop_range_enter);
        connect(game, &Game::watched_entity_leaves_range, this, &ECChaser::on_stop_range_leave);
    }
}

void ECChaser::chase_step() {
    /// if whats being chased has died, stop chasing
    /// if the thing chasing has died, stop chasing
    if (target_entity_.isNull() || entity_controlled() == nullptr) {
        stop_chasing();
        return;
    }

    /// make sure entity and one being chased are in a map
    Map *entitys_map = entity_controlled()->map();
    Map *chase_victims_map = target_entity_->map();
    assert(entitys_map != nullptr && chase_victims_map != nullptr);

    /// order to move towards chase victim :P
    if (!paused_) {
        path_mover_->move_entity(target_entity_->pos());
    }
}

void ECChaser::connect_to_target_signals() {
    // disconnect(0, &QObject::destroyed, this, &ECChaser::on_chased_entity_die);
    connect(target_entity_, &QObject::destroyed, this, &ECChaser::on_chased_entity_die);

    // disconnect(0, &Entity::map_left, this, &ECChaser::on_chased_entity_leave_map);
    connect(target_entity_, &Entity::map_left, this, &ECChaser::on_chased_entity_leave_map);

    /// listen to when the target entity enters/leaves stop distance (so we can stop moving)
    entity_controlled()->map()->game()->add_watched_entity(target_entity_, entity_controlled(), stop_distance_);
}

void ECChaser::disconnect_from_target_signals() {
    // disconnect(0, &QObject::destroyed, this, &ECChaser::on_chased_entity_die);
    // disconnect(0, &Entity::map_left, this, &ECChaser::on_chased_entity_leave_map);
    entity_controlled()->map()->game()->remove_watched_entity(target_entity_, entity_controlled());
}
