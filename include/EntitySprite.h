#pragma once

#include "PlayingAnimationInfo.h"
#include "Vendor.h"

class QGraphicsItem;
class QRectF;
class QPixmap;

namespace cute {

class Sprite;

/// Represents the visual aspect of an Entity.
/// An EntitySprite has a facing angle. When it is asked to play one of its animations, it
/// will ensure that the frames chosen to play are facing its facing angle. If the EntitySprite
/// is currently playing an animation and it is asked to change its facing angle, it will
/// continue playing the animation but at the new angle.
///
/// How to sub class properly
/// =========================
/// Entities go in Maps. A Map is really just a wrapper around a QGraphicsScene and an Entity is a
/// wrapper around a bunch of QGraphicsItems (more specifically, the EntitySprite of an Entity is
/// the wrapper around a bunch of QGraphicsItems). Therefore, when an Entity is inserted into a
/// Map, we need to insert the underlying QGraphicsItem representation of the Entity into the
/// underlying QGraphicsScene representation of the Map. Therefore, we need a way for the Map class
/// to access the underlying QGraphicsItem of the Entity's EntitySprite. Thus, every EntitySprite
/// shall have a pointer to its QGraphicsItem representation (called sprite_). Sub classes
/// must ensure that this pointer points to the proper QGraphicsItem that visually represents that
/// concrete EntitySprite. As the EntitySprite is rotated, you must ensure that the QGraphicsItem's
/// visual always represents the current angle.

class EntitySprite : public QObject {
    Q_OBJECT

    /// map needs access to the underlying QGraphicsItem for the reason mentioned in
    /// "Implementation Details" section of this class's API documentation.
    friend class Map;

    /// Entity::set_pos() needs to set the position of the underlying QGraphicsItem
    friend class Entity;

public:
    EntitySprite();
    EntitySprite(const QPixmap &pixmap);
    virtual ~EntitySprite();

    void set_facing_angle(double angle);
    double facing_angle() const { return facing_angle_; }

    virtual double actual_facing_angle() const = 0;

    virtual QRectF bounding_box() const;

    virtual bool has_animation(const std::string &animation) const = 0;

    /// Throws (assertion error) if the specified animation does not exist.
    /// If the specified animation is already playing (at current angle), does nothing
    /// (so if you want to restart, call stop() then play() again).
    /// If any other animation is currently playing, it will be stop()ed first.
    virtual void play(const std::string &animation, int num_times_to_play, int fps_to_play_at,
                      int starting_frame_number) = 0;

    void play_then_go_back_to_old_animation(const std::string animation, int num_times_to_play, int fps_to_play_at,
                                            int starting_frame_number);

    /// Returns some information about the currently playing animation.
    /// Call PlayingAnimationInfo::isNone() on the returned object to find out if no animation was playing.
    virtual PlayingAnimationInfo playing_animation() = 0;

    virtual void stop() = 0;

    virtual QPixmap currently_displayed_frame() const = 0;

    void scale(double scale);

signals:
    void frame_switched(EntitySprite *sender, int from_frame_number, int to_frame_number);
    void animation_finished(EntitySprite *sender, std::string animation);
    void animation_finished_completely(EntitySprite *sender, std::string animation);

public slots:
    void on_temp_animation_done(EntitySprite *sender, std::string animation);

protected:
    virtual void set_facing_angle_(double angle) = 0;

protected:
    double facing_angle_;

    Sprite *sprite_ = nullptr;

    PlayingAnimationInfo animation_playing_before_;
};

} // namespace cute
