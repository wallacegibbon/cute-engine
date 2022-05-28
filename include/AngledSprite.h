#pragma once

#include "EntitySprite.h"
#include "Vendor.h"

class QRectF;
class QPixmap;

namespace cute {

class Sprite;
class SpriteSheet;
class Node;

class AngledSprite : public EntitySprite {
    Q_OBJECT

public:
    AngledSprite();

    void add_frame(const QPixmap &frame, const std::string &animation, int angle);
    void add_frames(int angle, std::string animation, const SpriteSheet &sprite_sheet, const Node &from,
                    const Node &to);

    double actual_facing_angle() const override { return actual_facing_angle_; }
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
    void prepare_animation_angle(const std::string &animation, int angle);

private:
    std::string playing_animation_;
    int num_times_to_play_;
    int fps_to_play_at_;

    std::unordered_map<std::string, std::vector<int>> animation_to_angle_;

    double actual_facing_angle_;
};

} // namespace cute
