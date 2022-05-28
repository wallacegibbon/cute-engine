#include "RainWeather.h"
#include "Game.h"
#include "Map.h"
#include "Sound.h"
#include "Sprite.h"
#include "Utilities.h"

using namespace cute;

RainWeather::RainWeather(QPixmap rain_graphic, int num_of_rain_graphics, int fall_down_speed, int rain_step_size,
                         int splash_step_freq, int num_splash_per_step, double rain_initial_opacity,
                         double rain_max_opacity, double rain_opacity_step_size, int rain_initial_to_max_opacity_time,
                         double splash_initial_opacity, double splash_max_opacity, double splash_opacity_step_size,
                         int splash_initial_to_final_opacity_time)
        : rain_graphic_(rain_graphic), num_of_rain_graphics_(num_of_rain_graphics), fall_down_speed_(fall_down_speed),
          rain_step_size_(rain_step_size), splash_step_freq_(splash_step_freq),
          num_splash_per_step_(num_splash_per_step), rain_initial_opacity_(rain_initial_opacity),
          rain_max_opacity_(rain_max_opacity), rain_opacity_step_size_(rain_opacity_step_size),
          rain_initial_to_max_opacity_time_(rain_initial_to_max_opacity_time),
          splash_initial_opacity_(splash_initial_opacity), splash_max_opacity_(splash_max_opacity),
          splash_opacity_step_size_(splash_opacity_step_size),
          splash_initial_to_max_opacity_time_(splash_initial_to_final_opacity_time),
          current_splash_opacity_(splash_initial_opacity), current_rain_opacity_(rain_initial_opacity) {

    rain_move_timer_ = new QTimer(this);
    rain_opacity_timer_ = new QTimer(this);
    create_splash_timer_ = new QTimer(this);
    splash_opacity_timer_ = new QTimer(this);

    connect(rain_move_timer_, &QTimer::timeout, this, &RainWeather::on_rain_move_step);
    connect(rain_opacity_timer_, &QTimer::timeout, this, &RainWeather::on_rain_opacity_step);
    connect(create_splash_timer_, &QTimer::timeout, this, &RainWeather::on_create_splash_step);
    connect(splash_opacity_timer_, &QTimer::timeout, this, &RainWeather::on_splash_opacity_step);

    rain_sound_ = new Sound("qrc:/cute-engine-builtin/resources/sounds/rain.ogg", this);

    /// create some rain graphics
    for (int i = 0, n = num_of_rain_graphics_; i < n; i++) {
        QGraphicsPixmapItem *rain = new QGraphicsPixmapItem(rain_graphic_);
        rains_.push_back(rain);
    }
}

RainWeather::~RainWeather() {
    /// clean up rain graphics
    if (map_) {
        for (QGraphicsPixmapItem *rain : rains_) {
            map_->scene()->removeItem(rain);
            delete rain;
        }
    }
}

void RainWeather::start_() {
    /// add rain graphics to scene/set their opacities
    for (QGraphicsPixmapItem *rain : rains_) {
        map_->scene()->addItem(rain);
        rain->setOpacity(rain_initial_opacity_);
    }

    rain_sound_->set_volume(0);
    rain_sound_->play(-1);

    start_timers();
}

void RainWeather::stop_() {
    rain_move_timer_->stop();
    create_splash_timer_->stop();

    /// remove rain graphics from scene (splashes remove themselves after playing)
    for (QGraphicsPixmapItem *rain : rains_) {
        map_->scene()->removeItem(rain);
    }

    rain_sound_->stop();
}

void RainWeather::resume_() { start_timers(); }

void RainWeather::pause_() {
    rain_move_timer_->stop();
    rain_opacity_timer_->stop();
    create_splash_timer_->stop();
    splash_opacity_timer_->stop();
}

/// Executed periodically to move the rain graphics down.
/// Will also move them back up (at a random x,y) when far down enough.
void RainWeather::on_rain_move_step() {
    Game *maps_game = map_->game();

    double screen_bottom_y = maps_game->cam().bottom();
    for (QGraphicsPixmapItem *rain : rains_) {
        /// move down
        rain->moveBy(0, rain_step_size_);

        /// move back up if too far down
        if (rain->y() > screen_bottom_y) {
            double y = rand() % 700 - 700;                                     /// b/w -700 and 0
            double x = rand() % ((int)(maps_game->cam().width()) + 400) - 200; /// -200 -> cam_width+200
            rain->setPos(maps_game->map_to_map(QPoint(x, y)));
        }
    }
}

void RainWeather::on_rain_opacity_step() {
    if (current_rain_opacity_ < rain_max_opacity_) {
        current_rain_opacity_ += rain_opacity_step_size_;
        for (QGraphicsPixmapItem *rain : rains_) {
            rain->setOpacity(current_rain_opacity_);
        }
        rain_sound_->set_volume(rain_sound_->volume() + 1);
    } else {
        rain_opacity_timer_->disconnect();
    }
}

void RainWeather::on_create_splash_step() {
    Game *maps_game = map_->game();

    for (int i = 0, n = num_splash_per_step_; i < n; i++) {
        Sprite *splash = new Sprite();
        splash->setOpacity(current_splash_opacity_);
        splash->add_frames(":/cute-engine-builtin/resources/graphics/effects/splash", 4, "splash", "splash");
        double x = rand() % ((int)map_->game()->cam().width());  /// 0 - cam_width
        double y = rand() % ((int)map_->game()->cam().height()); /// 0 - cam_height
        QPointF pos = map_->game()->map_to_map(QPoint(x, y));
        map_->play_once(splash, "splash", 50, pos);
    }
}

void RainWeather::on_splash_opacity_step() {
    if (current_splash_opacity_ < splash_max_opacity_) {
        current_splash_opacity_ += splash_opacity_step_size_;
    } else {
        splash_opacity_timer_->disconnect();
    }
}

void RainWeather::start_timers() {
    rain_move_timer_->start(s_to_ms(frequency(rain_step_size_, fall_down_speed_)));

    /// the time unit is ms
    double rain_opacity_rate = (rain_max_opacity_ - rain_initial_opacity_) / rain_initial_to_max_opacity_time_;
    rain_opacity_timer_->start(frequency(rain_opacity_step_size_, rain_opacity_rate));

    create_splash_timer_->start(splash_step_freq_);

    double splash_opacity_rate = (splash_max_opacity_ - splash_initial_opacity_) / splash_initial_to_max_opacity_time_;
    splash_opacity_timer_->start(frequency(splash_opacity_step_size_, splash_opacity_rate));
}
