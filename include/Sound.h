#pragma once

#include "Vendor.h"

namespace cute {

/// Represents a sound that can be played a certain number of times (including an infinite number of times).
/// The sound will emit a signal when its done playing however many times it has been asked to play.

class Sound : public QObject {
    Q_OBJECT
public:
    Sound(std::string file_path, QObject *parent = nullptr);

    void play(int num_of_times);
    void stop();

    void set_volume(int volume);
    void set_mute(bool tf);

    QMediaPlayer::State state();
    int volume();

signals:
    /// Emitted when the Sound is finished playing the number of times that it
    /// has been asked to play.
    /// @note This signal will never emit if the sound has been asked to play
    /// an infinite number of times (i.e. -1 is passed in play()).
    void finished(Sound *sound);

public slots:
    void on_state_changed();

private:
    QMediaPlayer *media_player_;
    int num_times_played_;
    int num_times_to_play_;
};

} // namespace cute
