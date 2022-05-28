#pragma once

#include "Sound.h"
#include "Vendor.h"

namespace cute {

class Map;
class Sound;

/// Represents a sound object you can put in a Map. The furter the camera of the Map is from the
/// sound object, the lower its volume. When the PositionalSound's Map is not being visualized
/// (i.e. its Map is not the one Game is currently rendering) it is muted. When its Map is being
/// visualized, it is unmuted. So, in summary, PositionalSounds can only be heard when their Map is
/// being visualized, and their volume depends on how far the camera is from them.
///
/// Exmaple usage:
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
/// PositionalSound* ps = new PositionalSound(soundFile,pos);
/// map->add_positional_sound(ps);
/// ps->play(3);
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @author Abdullah Aghazadah
/// @date 2/22/17
class PositionalSound : public QObject {
    Q_OBJECT
    friend class Map;

public:
    PositionalSound(Map *in_map, std::string file_path, QPointF pos);

    void play(int num_of_times);
    void stop();
    void set_volume(int volume);
    void set_pos(const QPointF &pos);
    QPointF pos();

public slots:
    void on_cam_moved(QPointF new_cam_pos);
    void on_map_visualized();
    void on_map_no_longer_visualized();

private:
    int get_calculated_volume();
    void set_map_(Map *map);

private:
    /// the map the sound is in
    Map *map_;

    /// the position of the sound
    QPointF pos_;

    int volume_ = 100;

    bool should_be_playing_;

    /// internal sound (used to actually play the sound)
    std::unique_ptr<Sound> sound_;
};

} // namespace cute
