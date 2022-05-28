#include "Animation.h"
#include "Node.h"
#include "SpriteSheet.h"
#include "Utilities.h"

using namespace cute;

Animation::Animation(QGraphicsItem *parent) : QGraphicsItem(parent) {
    pixmaps_ = std::vector<QPixmap>();
    current_pixmap_ = new QGraphicsPixmapItem(this);
    timer_ = new QTimer(this);
    is_playing_ = false;
    current_frame_ = 0;
    times_played_ = 0;
    times_to_play_ = 0;
    frames_per_second_ = 0;
    connect(timer_, &QTimer::timeout, this, &Animation::on_animation_step);
}

Animation::Animation(const QPixmap &pixmap, QGraphicsItem *parent) : Animation(parent) {
    add_frame(pixmap);
    current_pixmap_->setPixmap(pixmap);
}

Animation::Animation(const SpriteSheet &sprite_sheet, Node from, Node to, QGraphicsItem *parent) : Animation(parent) {
    add_frames(sprite_sheet, from, to);
    if (!pixmaps_.empty()) {
        current_pixmap_->setPixmap(pixmaps_[0]);
    }
}

void Animation::add_frame(const QPixmap &pixmap) { pixmaps_.push_back(pixmap); }

void Animation::add_frames(const SpriteSheet &sprite_sheet, const Node &start_cell, const Node &end_cell) {
    std::vector<QPixmap> frames = sprite_sheet.tiles_at(start_cell, end_cell);
    for (QPixmap &frame : frames) {
        add_frame(frame);
    }
}

void Animation::add_frames(std::string folder_path, int num_of_images, std::string image_prefix,
                           std::string file_extension) {
    for (int i = 0; i < num_of_images; i++) {
        std::string full_path = folder_path + "/" + image_prefix + std::to_string(i) + file_extension;
        add_frame(QPixmap(full_path.c_str()));
    }
}

/// If you want to change the number of times to play it or to change the frames per second,
/// pause the animation, then call this function again.
void Animation::play(int num_times_to_play, double FPS_to_play_at) {
    if (is_playing_) {
        return;
    }

    /// remove static frame (if there)
    current_pixmap_->setPixmap(QPixmap());

    times_to_play_ = num_times_to_play;
    frames_per_second_ = FPS_to_play_at;

    timer_->start(s_to_ms(1 / frames_per_second_));

    is_playing_ = true;
}

void Animation::pause() {
    timer_->stop();
    is_playing_ = false;
}

bool Animation::is_playing() { return is_playing_; }

QRectF Animation::bounding_rect() const { return current_pixmap_->boundingRect(); }

QPixmap Animation::current_frame() {
    if (pixmaps_.empty()) {
        return QPixmap();
    } else {
        return current_pixmap_->pixmap();
    }
}

void Animation::on_animation_step() {
    /// if we have played enough times, stop playing
    if (times_played_ >= times_to_play_ && times_to_play_ != -1) {
        emit finished(this);
        pause();
        return;
    }

    /// if there are no more frames, start again and increment times played
    if (current_frame_ >= pixmaps_.size()) {
        emit finished(this);
        current_frame_ = 0;
        times_played_++;
        return;
    }

    /// show the next frame
    current_pixmap_->setPixmap(pixmaps_[current_frame_]);
    current_frame_++;
}
