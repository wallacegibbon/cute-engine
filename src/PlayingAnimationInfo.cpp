#include "PlayingAnimationInfo.h"

using namespace cute;

int PlayingAnimationInfo::fps() const {
    assert(!is_none());
    return fps_;
}

int PlayingAnimationInfo::times_left_to_play() const {
    assert(!is_none());
    return times_left_to_play_;
}

int PlayingAnimationInfo::current_frame() const {
    assert(!is_none());
    return current_frame_;
}
