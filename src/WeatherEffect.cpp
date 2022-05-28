#include "WeatherEffect.h"
#include "Map.h"

using namespace cute;

void WeatherEffect::start() {
    assert(map_ != nullptr);
    assert(map_->game() != nullptr);
    assert(state_ != PAUSED);
    if (state_ == STARTED) {
        return;
    }
    state_ = STARTED;

    /// delegate actual starting to concrete WeatherEffect
    start_();
}

void WeatherEffect::stop() {
    assert(!map_.isNull());
    if (state_ == STOPPED) {
        return;
    }
    state_ = STOPPED;

    /// delegate actual stopping to concrete WeatherEffect
    stop_();
}

/// Does nothing if the WeatherEffect is already playing ("started").
/// Throws if the WeatherEffect is stopped (must call start() to start a stopped WeatherEffect).
void WeatherEffect::resume() {
    assert(map_ != nullptr);
    assert(state_ != STOPPED);

    if (state_ == STARTED) {
        return;
    }
    state_ = STARTED;

    /// let concrete WeatherEffect do the actual resuming
    resume_();
}

/// A paused WeatherEffect "freezes" in its current "frame".
void WeatherEffect::pause() {
    assert(map_ != nullptr);
    if (state_ == PAUSED || state_ == STOPPED) {
        return;
    }
    state_ = PAUSED;

    /// let concrete class pause the WeatherEffect ("freeze") it
    pause_();
}

void WeatherEffect::on_map_visualized_1() {
    if (state_ == PAUSED) {
        resume();
    }
}

void WeatherEffect::on_map_visualized_2() {
    if (state_ == STARTED) {
        pause();
    }
}

/// This function is called by Map::add_weather_effect().
/// A WeatherEffect that is already added to a Map cannot be added to another Map (or moved from one to another)
/// instead of moving it, simply delete it and create another one for the new map
void WeatherEffect::set_map_(Map &map) {
    assert(map_ == nullptr);
    map_ = &map;
    connect(map_, &Map::set_as_current_map, this, &WeatherEffect::on_map_visualized_1);
    connect(map_, &Map::unset_as_current_map, this, &WeatherEffect::on_map_visualized_2);
}
