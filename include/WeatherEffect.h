#pragma once

#include "Map.h"
#include "Vendor.h"

namespace cute {

/// Abstract class that represents a weather effect for a Map. To add a
/// WeatherEffect effect to a Map, first create an instance of the
/// WeatherEffect, then use Map::add_weather_effect(WeatherEffect*). You can
/// remove a WeatherEffect from a map by calling Map::remove_weather_effect().
///
/// To create your own weather effect, extend this class and define what
/// happens to the Map when the weather effect is started (start_()),
/// stopped (stop_()), paused (pause_()), and resumed (resume_()).
///
/// When a WeatherEffect is paused, it will simply "freeze" itself in its current "frame."
/// It will continue playing when its resumed.
///
/// A paused WeatherEffect should consume very little, if any, resource

class WeatherEffect : public QObject {
    Q_OBJECT

    /// Map needs to set map_ during Map::add_weather_effect(WeatherEffect*)
    /// (because a WeatherEffect operates on a Map)
    friend class Map;

public:
    /// clang-format off
    enum State { STOPPED, STARTED, PAUSED };
    /// clang-format on

    /// these functions use template method pattern to do some "skeletal work"
    /// then delegate to concrete class. They are virtual in case the skeletal work
    /// does not suit you. If you do decide to override them, please ensure that you
    /// abide by the contract it sets forth in its api doc. Clients depend on this contract.
    virtual void start();
    virtual void stop();
    virtual void resume();
    virtual void pause();

public slots:
    void on_map_visualized_1();
    void on_map_visualized_2();

protected:
    /// the implementation of these functions should start/stop the  WeatherEffect respectively.
    /// the template "skeletal" methods above have already handled a few things.
    virtual void start_() = 0;
    virtual void stop_() = 0;
    virtual void resume_() = 0;
    virtual void pause_() = 0;

    QPointer<Map> map_ = nullptr;
    State state_ = STOPPED;

private:
    void set_map_(Map &map);
};

} // namespace cute
