#include "PositionalSound.h"
#include "Game.h"
#include "Map.h"
#include "QtUtilities.h"
#include "Sound.h"

using namespace cute;

PositionalSound::PositionalSound(Map *in_map, std::string file_path, QPointF pos) : map_(in_map), pos_(pos) {
    sound_.reset(new Sound(file_path));
    set_map_(in_map);
}

/// Plays the PositionalSound the specified number of times (pass -1 for infinite).
/// PositionalSounds can only be heard if their Map is being visualized (but they continue to seek).s
/// If play() is called while the sound is already playing, it will simply restart right away.
void PositionalSound::play(int num_of_times) {
    sound_->play(num_of_times);

    /// if currently in a map that is in a game
    if (map_ != nullptr) {
        Game *game = map_->game();
        if (game != nullptr) {
            /// set volume based on distance from camera
            int vol = get_calculated_volume();
            sound_->set_volume(vol);
            return;
        }
    }

    /// if not in a map, or map not visualized by game, mute sound
    sound_->set_mute(true);
}

/// Sets the overall volume of the PositionalSound.
/// Note that the actual volume will still adjust as the camera is panned around.
void PositionalSound::set_volume(int volume) {
    volume_ = volume;
    sound_->set_volume(get_calculated_volume());
}

void PositionalSound::set_pos(const QPointF &pos) {
    pos_ = pos;
    sound_->set_volume(get_calculated_volume());
}

QPointF PositionalSound::pos() { return pos_; }

/// Executed when the camera moves on the PositionalSound's Map (I.e. when the camera pans).
/// Will adjust the volume of the sound accordingly.
void PositionalSound::on_cam_moved(QPointF new_cam_pos) { sound_->set_volume(get_calculated_volume()); }

/// Executed when the Map that the PositionalSound is in is visualized.
/// Will unmute the sound.
void PositionalSound::on_map_visualized() { sound_->set_mute(false); }

/// Executed when the Map that the PositionalSound is in is no longer visualized.
/// Will mute the sound.
void PositionalSound::on_map_no_longer_visualized() { sound_->set_mute(true); }

int PositionalSound::get_calculated_volume() {
    /// fcn cannot be called if map_ is null or is not being visualized
    assert(map_ != nullptr);
    Game *game = map_->game();
    assert(game != nullptr);

    /// rate at which volume falls as distance grows
    const double FALLOFF_SLOPE = 0.0005;

    double dist_from_camera = QtUtils::distance(game->center_cam_pos(), pos_);
    double some_frac = -FALLOFF_SLOPE * dist_from_camera + 1;
    if (some_frac < 0) {
        some_frac = 0;
    } else if (some_frac > 1) {
        some_frac = 1;
    }
    return some_frac * volume_;
}

void PositionalSound::set_map_(Map *map) {
    map_ = map;
    /// if map is currently being visualized, unmute sound
    if (map_->game() != nullptr) {
        sound_->set_volume(get_calculated_volume());
        sound_->set_mute(false);
    }
    disconnect(map_, &Map::cam_moved, this, &PositionalSound::on_cam_moved);
    disconnect(map_, &Map::set_as_current_map, this, &PositionalSound::on_map_visualized);
    disconnect(map_, &Map::unset_as_current_map, this, &PositionalSound::on_map_no_longer_visualized);
    connect(map_, &Map::cam_moved, this, &PositionalSound::on_cam_moved);
    connect(map_, &Map::set_as_current_map, this, &PositionalSound::on_map_visualized);
    connect(map_, &Map::unset_as_current_map, this, &PositionalSound::on_map_no_longer_visualized);
}
