#pragma once

#include "Vendor.h"
#include "WeatherEffect.h"

class QGraphicsPixmapItem;
class QTimer;

namespace cute {

class SnowWeather : public WeatherEffect {
    Q_OBJECT

public:
    SnowWeather();
    ~SnowWeather();

    void start_() override;
    void stop_() override;
    void resume_() override;
    void pause_() override;

public slots:
    void on_snow_step_globular();
    void on_snow_step_linear();

private:
    void start_timers();

private:
    QTimer *globular_snow_timer_;
    QTimer *linear_snow_timer_;
    std::vector<QGraphicsPixmapItem *> globular_snows_;
    QGraphicsPixmapItem *snow1_ = nullptr;
    QGraphicsPixmapItem *snow2_ = nullptr;
    bool started_;
    bool initial_globular_ = true;
    bool initial_linear_ = true;
};

} // namespace cute
