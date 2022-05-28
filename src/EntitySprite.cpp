#include "EntitySprite.h"
#include "Sprite.h"

using namespace cute;

EntitySprite::EntitySprite() : sprite_(new Sprite()) {}
EntitySprite::EntitySprite(const QPixmap &pixmap) : sprite_(new Sprite(pixmap)) {}

EntitySprite::~EntitySprite() { delete sprite_; }

void EntitySprite::set_facing_angle(double angle) {
    facing_angle_ = angle;
    set_facing_angle_(angle);
}

QRectF EntitySprite::bounding_box() const { return sprite_->bounding_rect(); }

void EntitySprite::play_then_go_back_to_old_animation(const std::string animation, int num_times_to_play,
                                                      int fps_to_play_at, int starting_frame_number) {
    animation_playing_before_ = playing_animation();
    connect(this, &EntitySprite::animation_finished_completely, this, &EntitySprite::on_temp_animation_done);
    play(animation, num_times_to_play, fps_to_play_at, starting_frame_number);
}

void EntitySprite::scale(double scale) { sprite_->set_scale(scale); }

void EntitySprite::on_temp_animation_done(EntitySprite *sender, std::string animation) {
    if (animation_playing_before_.is_none()) {
        return;
    }
    disconnect(this, &EntitySprite::animation_finished_completely, this, &EntitySprite::on_temp_animation_done);
    play(animation_playing_before_.name(), animation_playing_before_.times_left_to_play(),
         animation_playing_before_.fps(), animation_playing_before_.current_frame());
}
