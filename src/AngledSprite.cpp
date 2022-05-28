#include "AngledSprite.h"
#include "Sprite.h"
#include "Utilities.h"
#include "stl_helper.h"

using namespace cute;

AngledSprite::AngledSprite() {
    connect(sprite_, &Sprite::animation_finished, this, &AngledSprite::on_internal_sprite_animation_finished);
    connect(sprite_, &Sprite::animation_finished_completely, this,
            &AngledSprite::on_internal_sprite_animation_completely_finished);
    connect(sprite_, &Sprite::frame_switched, this, &AngledSprite::on_internal_sprite_frame_switched);
}

void AngledSprite::prepare_animation_angle(const std::string &animation, int angle) {
    assert(animation != "");
    assert(!stl_helper::contain_digit(animation));
    if (!stl_helper::contains(animation_to_angle_, animation)) {
        animation_to_angle_[animation] = std::vector<int>();
    }
    std::vector<int> angles = animation_to_angle_[animation];
    if (!stl_helper::contains(animation_to_angle_[animation], angle)) {
        animation_to_angle_[animation].push_back(angle);
    }
}

void AngledSprite::add_frame(const QPixmap &frame, const std::string &animation, int angle) {
    prepare_animation_angle(animation, angle);
    sprite_->add_frame(frame, animation + std::to_string(angle));
}

void AngledSprite::add_frames(int angle, std::string animation, const SpriteSheet &sprite_sheet, const Node &from_node,
                              const Node &to_node) {
    prepare_animation_angle(animation, angle);
    sprite_->add_frames(from_node, to_node, sprite_sheet, animation + std::to_string(angle));
}

bool AngledSprite::has_animation(const std::string &animation) const {
    return animation_to_angle_.find(animation) != animation_to_angle_.end();
}

void AngledSprite::play(const std::string &animation, int num_times_to_play, int fps_to_play_at,
                        int starting_frame_number) {
    assert(has_animation(animation));

    playing_animation_ = animation;
    num_times_to_play_ = num_times_to_play;
    fps_to_play_at_ = fps_to_play_at;

    std::vector<int> angle = animation_to_angle_[animation];
    int closest = closest_angle(angle, facing_angle());

    sprite_->play(animation + std::to_string(closest), num_times_to_play, fps_to_play_at, starting_frame_number);

    actual_facing_angle_ = closest;
}

void AngledSprite::stop() {
    sprite_->stop();
    playing_animation_ = "";
}

PlayingAnimationInfo AngledSprite::playing_animation() {
    if (playing_animation_ == "") {
        return PlayingAnimationInfo();
    }
    int times_left_to_play = sprite_->playing_animation().times_left_to_play();
    int current_frame = sprite_->current_frame_number();
    return PlayingAnimationInfo(playing_animation_, fps_to_play_at_, times_left_to_play, current_frame);
}

QPixmap AngledSprite::currently_displayed_frame() const { return sprite_->current_frame(); }

QRectF AngledSprite::bounding_box() const { return sprite_->bounding_rect(); }

/// TODO: try to use rvalue-reference to eliminate the copy of the returned str_without_num.
std::string remove_tail_number(const std::string &original_str) {
    std::string str_without_num;
    for (char c : original_str) {
        if (!isdigit(c)) {
            str_without_num.push_back(c);
        }
    }
    return str_without_num;
}

void AngledSprite::on_internal_sprite_animation_finished(Sprite *sender, std::string animation) {
    emit animation_finished(this, remove_tail_number(animation));
}

void AngledSprite::on_internal_sprite_animation_completely_finished(Sprite *sender, std::string animation) {
    emit animation_finished_completely(this, remove_tail_number(animation));
    playing_animation_ = "";
}

void AngledSprite::on_internal_sprite_frame_switched(Sprite *sender, int from_frame_num, int to_frame_num) {
    emit frame_switched(this, from_frame_num, to_frame_num);
}

/// if currently playing an animation, find version closest to angle and start playing that version
/// from current frame index
void AngledSprite::set_facing_angle_(double angle) {
    int frame_idx = sprite_->current_frame_number();
    if (sprite_->playing_animation().name() == "") {
        return;
    }
    play(playing_animation_, sprite_->playing_animation().times_left_to_play(), sprite_->playing_animation().fps(),
         frame_idx);
}
