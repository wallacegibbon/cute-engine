#include "Sound.h"

using namespace cute;

Sound::Sound(std::string file_path, QObject *parent) : QObject(parent) {
    media_player_ = new QMediaPlayer(this);
    media_player_->setMedia(QUrl(file_path.c_str()));

    num_times_played_ = 0;
    num_times_to_play_ = 0;

    connect(media_player_, &QMediaPlayer::stateChanged, this, &Sound::on_state_changed);
}

/// Plays the sound the specified number of times. Pass -1 to play an infinite number of times.
/// If the Sound is already playing, it will simply *restart* playing right away.
void Sound::play(int num_of_times) {
    num_times_to_play_ = num_of_times;
    num_times_played_ = 1;

    if (media_player_->state() == QMediaPlayer::PlayingState) {
        media_player_->stop();
    }

    media_player_->play();
}

void Sound::stop() {
    num_times_played_ = 0;
    num_times_to_play_ = 0;
    media_player_->stop();
}

/// Sets the volume of the sound, from 0-100.
void Sound::set_volume(int volume) { media_player_->setVolume(volume); }

void Sound::set_mute(bool tf) { media_player_->setMuted(tf); }

QMediaPlayer::State Sound::state() { return media_player_->state(); }

/// Returns the volume of the Sound. 0-100.
int Sound::volume() { return media_player_->volume(); }

void Sound::on_state_changed() {
    if (media_player_->state() != QMediaPlayer::StoppedState) {
        return;
    }

    if (num_times_played_ < num_times_to_play_ || num_times_to_play_ == -1) {
        media_player_->setPosition(0);
        media_player_->play();
        num_times_played_++;
    } else {
        emit finished(this);
    }
}
