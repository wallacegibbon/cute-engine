#include "Sprite.h"
#include "Node.h"
#include "PlayingAnimationInfo.h"
#include "SpriteSheet.h"
#include "Utilities.h"

using namespace cute;

Sprite::Sprite(const QPixmap &pixmap, QGraphicsItem *parent) : QGraphicsItem(parent) {
    pixmap_item_ = new QGraphicsPixmapItem(pixmap, this);
    timer_ = new QTimer(this);
    current_frame_ = 0;
    times_played_ = 0;
    times_to_play_ = 0;

    playing_animation_ = "";
    playing_animation_FPS_ = -1;
}

void Sprite::play(std::string animation, int times_to_play, double frames_per_second, int starting_frame_number) {
    /// make sure the animation exists
    assert(animation_.find(animation) != animation_.end());

    /// do nothing if we're already playing the specified animation
    if (playing_animation().name() == animation) {
        return;
    }
    /// stop the currently playing animation except for the die animation.
    /// The die animation should never be stopped here. Some clean-ups depends on the signal
    if (playing_animation_.substr(0, 3) == std::string("die")) {
        return;
    } else {
        stop();
    }

    playing_animation_ = animation;
    playing_animation_FPS_ = frames_per_second;
    animation_pixmaps_ = animation_[animation];
    times_played_ = 0;
    current_frame_ = starting_frame_number;
    times_to_play_ = times_to_play;

    on_next_frame();
    connect(timer_, &QTimer::timeout, this, &Sprite::on_next_frame);
    timer_->start(s_to_ms(1 / frames_per_second));
}

void Sprite::play_then_go_back_to_old_animation(std::string animation, int num_times_to_play, double frames_per_second,
                                                int starting_frame_number) {
    animation_playing_last_ = playing_animation();
    /// prevent double connect
    disconnect(this, &Sprite::animation_finished_completely, this, &Sprite::on_temporary_play_done);
    connect(this, &Sprite::animation_finished_completely, this, &Sprite::on_temporary_play_done);
    play(animation, num_times_to_play, frames_per_second, starting_frame_number);
}

QRectF Sprite::bounding_rect() const { return pixmap_item_->boundingRect(); }

bool Sprite::has_animation(std::string animation) const { return animation_.find(animation) != animation_.end(); }

std::vector<std::string> Sprite::animations() const {
    std::vector<std::string> results;
    for (const auto &animation_pixmap_pair : animation_) {
        results.push_back(animation_pixmap_pair.first);
    }
    return results;
}

PlayingAnimationInfo Sprite::playing_animation() const {
    PlayingAnimationInfo info(playing_animation_, playing_animation_FPS_, playing_animation_times_left_to_play(),
                              current_frame_number());
    return info;
}

/// Returns the number of times we have left to play the currently playing animation.
/// Sprite::play() allows you to pass in the number of times you want the animation to be played.
/// This function can tell you how many times you have left to play.
/// Returns -1 if the animation was asked to play an infinite number of times
/// (thus it has an infinite number of times left to play). Includes the current run.
int Sprite::playing_animation_times_left_to_play() const {
    if (times_to_play_ == -1) {
        return -1;
    } else {
        return times_to_play_ - times_played_;
    }
}

QPixmap Sprite::current_frame() const { return pixmap_item_->pixmap(); }

/// If the animation already exists, the pixmap will simply be added as the next frame in the animation.
/// If the animation does not exist, it will be created with the pixmap being its first frame.
void Sprite::add_frame(QPixmap frame, std::string animation) {
    assert(animation != "");

    if (animation_.find(animation) == animation_.end()) {
        animation_[animation] = std::vector<QPixmap>();
    }
    animation_[animation].push_back(frame);
}

void Sprite::add_frames(std::string folder, int num_of_images, const std::string &image_prefix,
                        const std::string &animation) {
    for (int i = 0; i < num_of_images; i++) {
        std::string full_path = folder + "/" + image_prefix + std::to_string(i) + ".png";
        add_frame(QPixmap(full_path.c_str()), animation);
    }
}

void Sprite::add_frame(const Node &tile, const SpriteSheet &sprite_sheet, std::string animation) {
    QPixmap pixmap = sprite_sheet.tile_at(tile);
    add_frame(pixmap, animation);
}

void Sprite::add_frames(const Node &top_left_cell, const Node &bottom_right_cell, const SpriteSheet &sprite_sheet,
                        std::string animation) {
    std::vector<QPixmap> pixmaps = sprite_sheet.tiles_at(top_left_cell, bottom_right_cell);
    for (QPixmap pixmap : pixmaps) {
        add_frame(pixmap, animation);
    }
}

void Sprite::set_pixmap(QPixmap pixmap) { pixmap_item_->setPixmap(pixmap); }

void Sprite::stop() {
    playing_animation_ = "";
    playing_animation_FPS_ = -1;
    timer_->disconnect();
}

/// this method is only meant to be used by the play() method.
void Sprite::on_next_frame() {
    if (times_to_play_ != -1 && times_played_ >= times_to_play_) {
        emit animation_finished(this, playing_animation().name());
        emit animation_finished_completely(this, playing_animation().name());
        stop();
        return;
    }
    if (current_frame_ >= animation_pixmaps_.size()) {
        emit animation_finished(this, playing_animation().name());
        current_frame_ = 0;
        times_played_++;
    }
    set_pixmap(animation_pixmaps_[current_frame_]);
    int from_frame = current_frame_ == 0 ? animation_pixmaps_.size() - 1 : current_frame_ - 1;
    emit frame_switched(this, from_frame, current_frame_);
    current_frame_++;
}

/// Executed when a "temporary" play has just finished.
/// A temporary play initiates due to play_then_go_back_to_old_animation(). Will play the old playing animation.
void Sprite::on_temporary_play_done(Sprite *sender, std::string animation) {
    /// client takes care of preventing double connects
    if (animation_playing_last_.is_none()) {
        return;
    }
    play(animation_playing_last_.name(), animation_playing_last_.times_left_to_play(), animation_playing_last_.fps(),
         animation_playing_last_.current_frame());
}
