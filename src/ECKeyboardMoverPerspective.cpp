#include "ECKeyboardMoverPerspective.h"
#include "EntitySprite.h"
#include "Game.h"
#include "Map.h"
#include "QTimer"
#include "Sprite.h"
#include "Utilities.h"

using namespace cute;

ECKeyboardMoverPerspective::ECKeyboardMoverPerspective(Entity *entity) : EntityController(entity) {
    assert(entity != nullptr);
    move_timer_ = new QTimer(this);
    connect(move_timer_, &QTimer::timeout, this, &ECKeyboardMoverPerspective::on_move_step);
    move_timer_->start(s_to_ms(frequency(step_size_, entity_controlled()->speed())));
}

void ECKeyboardMoverPerspective::step_on_relative_angle(int angle) {
    Entity *entity = entity_controlled();
    assert(entity != nullptr);

    QLineF line(entity->pos(), QPoint(0, 0));
    line.setAngle(360 - entity->facing_angle());
    line.setLength(step_size_);
    if (angle != 0) {
        line.setAngle(line.angle() + angle);
    }

    QPointF pt(entity->pos().x() + line.dx(), entity->pos().y() + line.dy());

    if (entity->can_fit(pt)) {
        entity->set_pos(pt);
        play_animation_if_no_other_playing("walk");
    } else {
        entity->set_pos(entity->pos());
    }
}

void ECKeyboardMoverPerspective::on_move_step() {
    /// if the entity has been destroyed, stop
    Entity *entity = entity_controlled();
    if (entity == nullptr) {
        move_timer_->disconnect();
        return;
    }
    /// if currently not in a Map, do nothing
    Map *entitys_map = entity->map();
    if (entitys_map == nullptr) {
        return;
    }
    /// if entitys_map is not in a Game, do noting
    Game *entitys_game = entitys_map->game();
    if (entitys_game == nullptr) {
        return;
    }

    std::set<int> key_pressed = entitys_game->keys_pressed();

    bool cmd_forward = key_pressed.count(Qt::Key_W) + key_pressed.count(Qt::Key_Up);
    bool cmd_backward = key_pressed.count(Qt::Key_S) + key_pressed.count(Qt::Key_Down);
    bool cmd_left = key_pressed.count(Qt::Key_A) + key_pressed.count(Qt::Key_Left);
    bool cmd_right = key_pressed.count(Qt::Key_D) + key_pressed.count(Qt::Key_Right);

    if (cmd_forward) {
        step_on_relative_angle(0);
    }
    if (cmd_backward) {
        step_on_relative_angle(180);
    }
    if (cmd_left) {
        step_on_relative_angle(90);
    }
    if (cmd_right) {
        step_on_relative_angle(-90);
    }
    if (!cmd_forward && !cmd_left && !cmd_backward && !cmd_right) {
        play_animation_if_no_other_playing("stand");
    }
}

void ECKeyboardMoverPerspective::play_animation_if_no_other_playing(std::string anim) {
    Entity *entity = entity_controlled();
    EntitySprite *entitys_sprite = entity->sprite();
    std::string playing_anim = entitys_sprite->playing_animation().name();
    if (entitys_sprite->has_animation(anim) &&
        (playing_anim == "" || playing_anim == "stand" || playing_anim == "walk")) {
        entity->sprite()->play(anim, -1, 10, 0);
    }
}
