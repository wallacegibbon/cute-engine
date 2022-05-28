#pragma once

#include "EntitySprite.h"
#include "Vendor.h"

namespace cute {

class Sprite;
class SpriteSheet;
class Node;

class TopDownSprite : public EntitySprite {
    Q_OBJECT

public:
    TopDownSprite();
    TopDownSprite(const QPixmap &pixmap);

    void add_frames(std::string animation, const SpriteSheet &sprite_sheet, const Node &from, const Node &to);
    void add_frames(std::string resource_folder, int num_of_images, std::string image_prefix);
    void add_frame(const QPixmap &frame, const std::string &animation);

    double actual_facing_angle() const override;

    QRectF bounding_box() const override;

    bool has_animation(const std::string &animation) const override;

    QPixmap currently_displayed_frame() const override;

    void play(const std::string &animation, int num_times_to_play, int fps_to_play_at,
              int starting_frame_number) override;

    void stop() override;

    PlayingAnimationInfo playing_animation() override;

public slots:
    void on_internal_sprite_animation_finished(Sprite *sender, std::string animation);
    void on_internal_sprite_animation_completely_finished(Sprite *sender, std::string animation);
    void on_internal_sprite_frame_switched(Sprite *sender, int from_frame_num, int to_frame_num);

private:
    void set_facing_angle_(double angle) override;
    void common_initialize();
};

} // namespace cute
