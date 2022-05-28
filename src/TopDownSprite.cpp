#include "TopDownSprite.h"
#include "Node.h"
#include "Sprite.h"
#include "SpriteSheet.h"
#include "stl_helper.h"

using namespace cute;

TopDownSprite::TopDownSprite() { common_initialize(); }
TopDownSprite::TopDownSprite(const QPixmap &pixmap) : EntitySprite(pixmap) { common_initialize(); }

void TopDownSprite::common_initialize() {
    connect(sprite_, &Sprite::animation_finished, this, &TopDownSprite::on_internal_sprite_animation_finished);
    connect(sprite_, &Sprite::animation_finished_completely, this,
            &TopDownSprite::on_internal_sprite_animation_completely_finished);
    connect(sprite_, &Sprite::frame_switched, this, &TopDownSprite::on_internal_sprite_frame_switched);
}

void TopDownSprite::add_frames(std::string animation, const SpriteSheet &sprite_sheet, const Node &from,
                               const Node &to) {
    assert(!stl_helper::contain_digit(animation));
    sprite_->add_frames(from, to, sprite_sheet, animation);
}

void TopDownSprite::add_frames(std::string resource_folder, int num_of_images, std::string image_prefix) {
    assert(!stl_helper::contain_digit(image_prefix));
    sprite_->add_frames(resource_folder, num_of_images, image_prefix, image_prefix);
}

void TopDownSprite::add_frame(const QPixmap &frame, const std::string &animation) {
    sprite_->add_frame(frame, animation);
}

double TopDownSprite::actual_facing_angle() const { return facing_angle(); }

QRectF TopDownSprite::bounding_box() const { return sprite_->bounding_rect(); }

bool TopDownSprite::has_animation(const std::string &animation) const { return sprite_->has_animation(animation); }

void TopDownSprite::play(const std::string &animation, int num_times_to_play, int fps_to_play_at,
                         int starting_frame_number) {
    sprite_->play(animation, num_times_to_play, fps_to_play_at, starting_frame_number);
}

void TopDownSprite::stop() { sprite_->stop(); }

PlayingAnimationInfo TopDownSprite::playing_animation() { return sprite_->playing_animation(); }

QPixmap TopDownSprite::currently_displayed_frame() const { return sprite_->current_frame(); }

void TopDownSprite::on_internal_sprite_animation_finished(Sprite *sender, std::string animation) {
    emit animation_finished(this, animation);
}

void TopDownSprite::on_internal_sprite_animation_completely_finished(Sprite *sender, std::string animation) {
    emit animation_finished_completely(this, animation);
}

void TopDownSprite::on_internal_sprite_frame_switched(Sprite *sender, int from_frame_num, int to_frame_num) {
    emit frame_switched(this, from_frame_num, to_frame_num);
}

void TopDownSprite::set_facing_angle_(double angle) { sprite_->set_rotation(angle); }
