#pragma once

#include "PlayingAnimationInfo.h"
#include "Vendor.h"

class QGraphicsPixmapItem;
class QTimer;

namespace cute {

class SpriteSheet;
class Node;

/// A QGraphicsItem that represents a bunch of animations that can be played.
/// @author Abdullah Aghazadah
///
/// Since a Sprite is a QGraphicsItem, it can be placed inside a QGraphicsScene. To add a frame to
/// an animation, use Sprite::add_frame(QPixmap,std::string). The specified pixmap will then be
/// added to the specified animation. If an animation of that name does not exist, it will be
/// created. If an animation of that name already exists, then the pixmap will be added as the next
/// image in the animation. To add a bunch of images from a folder to an animation use
/// Sprite::add_frames(std::string,int,std::string,std::string). You can also add tiles from a
/// SpriteSheet as animation frames in the Sprite.
///
/// A Sprite can play an animation a certain number of times by using Sprite::play(). A value of -1
/// times means that the animation will be played again and again forever.

class Sprite : public QObject, public QGraphicsItem {
    Q_OBJECT

public:
    Sprite(const QPixmap &pixmap, QGraphicsItem *parent = nullptr);
    Sprite(QGraphicsItem *parent = nullptr)
            : Sprite(QPixmap(":/cute-engine-builtin/resources/graphics/misc/default_entity.png"), parent) {}

    bool has_animation(std::string animation) const;
    std::vector<std::string> animations() const;
    PlayingAnimationInfo playing_animation() const;
    QPixmap current_frame() const;
    int current_frame_number() const { return current_frame_; }

    void add_frame(QPixmap frame, std::string animation);
    void add_frame(const Node &tile, const SpriteSheet &sprite_sheet, std::string animation);

    void add_frames(std::string folder, int num_of_images, const std::string &image_prefix,
                    const std::string &animation);
    void add_frames(const Node &top_left_cell, const Node &bottom_right_cell, const SpriteSheet &sprite_sheet,
                    std::string animation);

    void set_pixmap(QPixmap pixmap);
    void set_pos(const QPointF &pos) { setPos(pos); }
    void set_z_value(double value) { setZValue(value); }
    void set_scale(double scale) { setScale(scale); }
    void set_rotation(double angle) { setRotation(angle); }

    /// override the default methods in QGraphicsItem.
    /// the `paint` does nothing because Sprite doesn't need to paint anything,
    /// the currently displayed pixmap is painted via a QGraphicsPixmapItem
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override{};
    QRectF boundingRect() const override { return bounding_rect(); };

    virtual QRectF bounding_rect() const;

    void play(std::string animation, int times_to_play, double frames_per_second, int starting_frame_number = 0);

    void play_then_go_back_to_old_animation(std::string animation, int num_times_to_play, double frames_per_second,
                                            int starting_frame_number = 0);
    void stop();

public slots:
    void on_next_frame();
    void on_temporary_play_done(Sprite *sender, std::string animation);

signals:
    /// Emitted each time the frame switches due to an animation playing.
    void frame_switched(Sprite *sender, int from_frame_number, int to_frame_number);

    /// Emitted each time an animation has finished playing.
    /// Even if an animation was asked to play multiple (even infinite) number of times,
    /// this event will emit every time the last frame is reached.
    void animation_finished(Sprite *sender, std::string animation);

    /// Emitted when an animation has finished playing the number of times it was asked to play.
    /// The difference between this and animation_finished() is that this is only
    /// emitted when the animation has finished playing the number of times
    /// it was asked to play, where as animation_finished() will be emitted
    /// every single time the last frame is reached.
    void animation_finished_completely(Sprite *sender, std::string animation);

private:
    int playing_animation_times_left_to_play() const;

private:
    std::unordered_map<std::string, std::vector<QPixmap>> animation_;
    std::string playing_animation_;
    int playing_animation_FPS_;

    QGraphicsPixmapItem *pixmap_item_;
    std::vector<QPixmap> animation_pixmaps_;
    int current_frame_;
    int times_played_;
    int times_to_play_;
    QTimer *timer_;

    PlayingAnimationInfo animation_playing_last_;
};

} // namespace cute
