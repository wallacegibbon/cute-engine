#pragma once

#include "Vendor.h"
#include "WeatherEffect.h"

class QTimer;
class QGraphicsPixmapItem;

namespace cute {

class Sound;

class RainWeather : public WeatherEffect {
    Q_OBJECT

public:
    RainWeather(QPixmap rain_graphic = QPixmap(":/cute-engine-builtin/resources/graphics/effects/rain.png"),
                int num_of_rain_graphics = 150, int fall_down_speed = 1500, int rain_step_size = 100,
                int splash_step_freq = 150, int num_splash_per_step = 7, double rain_initial_opacity = 0.08,
                double rain_max_opacity = 0.5, double rain_opacity_step_size = 0.005,
                int rain_initial_to_max_opacity_time = 5000, double splash_initial_opacity = 0.08,
                double splash_max_opacity = 0.5, double splash_opacity_step_size = 0.002,
                int splash_initial_to_final_opacity_time = 5000);

    ~RainWeather();

    void start_() override;
    void stop_() override;
    void resume_() override;
    void pause_() override;

public slots:
    void on_rain_move_step();
    void on_rain_opacity_step();
    void on_create_splash_step();
    void on_splash_opacity_step();

private:
    void start_timers();

private:
    QTimer *rain_move_timer_;
    QTimer *rain_opacity_timer_;
    QTimer *create_splash_timer_;
    QTimer *splash_opacity_timer_;

    std::vector<QGraphicsPixmapItem *> rains_;
    Sound *rain_sound_;

    /// the graphic to use for the actual rain
    QPixmap rain_graphic_;

    /// number of rain graphics to randomly generate
    int num_of_rain_graphics_;

    /// speed at which the rain graphics fall down (in pixels per second)
    int fall_down_speed_;

    /// how much to move the rain down every step (determines movement granularity)
    int rain_step_size_;

    /// how often to create a group of splashes (in ms)
    int splash_step_freq_;

    /// how many splashes to create each time they are created
    int num_splash_per_step_;

    double rain_initial_opacity_;
    double rain_max_opacity_;
    double rain_opacity_step_size_;
    int rain_initial_to_max_opacity_time_;

    double splash_initial_opacity_;
    double splash_max_opacity_;
    double splash_opacity_step_size_;
    int splash_initial_to_max_opacity_time_;

    double current_rain_opacity_;
    double current_splash_opacity_;
};

} // namespace cute
