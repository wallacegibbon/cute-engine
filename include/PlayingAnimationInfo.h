#pragma once

#include "Vendor.h"

namespace cute {

/// Simple class that represents information about a currently playing animation.
class PlayingAnimationInfo {
public:
    PlayingAnimationInfo() : fps_(-1), times_left_to_play_(0), current_frame_(-1) {}

    PlayingAnimationInfo(const std::string &name, int fps, int times_left_to_play, int current_frame_)
            : animation_name_(name), fps_(fps), times_left_to_play_(times_left_to_play),
              current_frame_(current_frame_) {}

    /// Returns the name of the playing animation. ("" if no currently playing animation)
    std::string name() const { return animation_name_; }

    int fps() const;
    int times_left_to_play() const;
    int current_frame() const;

    /// Returns true if this struct represents no animation playing.
    bool is_none() const { return name() == ""; }

private:
    std::string animation_name_;
    int fps_;
    int times_left_to_play_;
    int current_frame_;
};

} // namespace cute
