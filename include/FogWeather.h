#pragma once

#include "Vendor.h"
#include "WeatherEffect.h"
#include <QVector2D>

class QTimer;
class QGraphicsPixmapItem;

namespace cute {

class FogWeather : public WeatherEffect {
    Q_OBJECT
public:
    FogWeather(QPixmap tileable_fog_graphic = QPixmap(":/cute-engine-builtin/resources/graphics/effects/fog.png"),
               int tile_width = 1024, int tile_height = 1024, double initial_opacity = 0.05, double final_opacity = 0.5,
               double opacity_fade_time_ms = 2000, double opacity_step_size = 0.005);
    ~FogWeather();

    void start_() override;
    void stop_() override;
    void resume_() override;
    void pause_() override;

    void set_fog_speed(double pixels_per_second);
    void set_fog_step_size(double num_pixels);

public slots:
    void on_opacity_step();
    void on_move_step();

private:
    void start_timers();

private:
    QTimer *opacity_timer_;
    QTimer *move_timer_;
    std::set<QGraphicsPixmapItem *> fog_squares_;
    int fog_picture_width_;
    int fog_picture_height_;

    double initial_opacity_;
    double max_opacity_;

    /// how long it takes for the opacity to go from initial to max (in ms)
    int opacity_fade_time_;

    /// granularity of opacity fading in
    double opacity_step_size_;

    /// how fast the fog moves in pixels per second
    double fog_speed_ = 50;

    /// granularity of movement for the fog
    double fog_step_size_ = 5;

    /// the direction the fog moves in
    QVector2D fog_direction_;

    QPixmap fog_picture_;

    QRectF fog_boundry_;
    double current_opacity_;
};

} // namespace cute
