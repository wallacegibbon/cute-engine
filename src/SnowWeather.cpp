#include "SnowWeather.h"
#include "Game.h"
#include "Map.h"
#include "RandomGenerator.h"

using namespace cute;

SnowWeather::SnowWeather() {
    globular_snow_timer_ = new QTimer(this);
    linear_snow_timer_ = new QTimer(this);
    connect(globular_snow_timer_, &QTimer::timeout, this, &SnowWeather::on_snow_step_globular);
    connect(linear_snow_timer_, &QTimer::timeout, this, &SnowWeather::on_snow_step_linear);
}

SnowWeather::~SnowWeather() {
    /// delete all snow graphics from the scene of the Map (if it has a Map)
    if (map_) {
        for (QGraphicsPixmapItem *snow : globular_snows_) {
            map_->scene()->removeItem(snow);
        }
    }

    map_->scene()->removeItem(snow1_);
    map_->scene()->removeItem(snow2_);

    delete snow1_;
    delete snow2_;
}

void SnowWeather::start_() {
    Game *maps_game = map_->game();

    /// create some globular snow
    for (int i = 0, n = 10; i < n; i++) {
        QGraphicsPixmapItem *snow =
                new QGraphicsPixmapItem(QPixmap(":/cute-engine-builtin/resources/graphics/effects/snow/snow0.png"));
        globular_snows_.push_back(snow);
        map_->scene()->addItem(snow);
        snow->setOpacity(0.05);
    }

    /// position snows at top of camera
    for (QGraphicsPixmapItem *snow : globular_snows_) {
        double offset_x = common_random_generator.rand_double(-300, maps_game->cam().width());
        double offset_y = common_random_generator.rand_double(0, maps_game->cam().height() * 2);
        snow->setPos(maps_game->cam().topLeft());
        snow->setX(snow->x() + offset_x);
        snow->setY(snow->y() - snow->boundingRect().height() - offset_y);
    }

    /// create linear snow (snow1 and snow2)
    QPixmap pm(":/cute-engine-builtin/resources/graphics/effects/snow/snow1.png");
    pm = pm.scaled(maps_game->cam().width(), pm.height());
    snow1_ = new QGraphicsPixmapItem(pm);
    snow2_ = new QGraphicsPixmapItem(pm);
    snow1_->setOpacity(0.05);
    snow2_->setOpacity(0.05);
    map_->scene()->addItem(snow1_);
    map_->scene()->addItem(snow2_);

    /// position snow1 and 2
    QPointF cam_pos = maps_game->cam().topLeft();
    snow1_->setPos(cam_pos);
    snow2_->setPos(cam_pos);
    snow2_->setY(snow2_->y() - snow2_->boundingRect().height());

    start_timers();
}

void SnowWeather::stop_() {
    globular_snow_timer_->stop();

    /// clean graphics
    for (QGraphicsPixmapItem *snow : globular_snows_) {
        map_->scene()->removeItem(snow);
    }
    globular_snows_.clear();

    /// remove snow graphics (if they have been created)
    if (snow1_ != nullptr) {
        map_->scene()->removeItem(snow1_);
    }
    if (snow2_ != nullptr) {
        map_->scene()->removeItem(snow2_);
    }
    globular_snow_timer_->stop();
    linear_snow_timer_->stop();
}

void SnowWeather::resume_() { start_timers(); }

void SnowWeather::pause_() {
    globular_snow_timer_->stop();
    linear_snow_timer_->stop();
}

void SnowWeather::on_snow_step_globular() {
    Game *maps_game = map_->game();

    double screen_bottom_y = map_->game()->center_cam_pos().y() + map_->game()->sceneRect().height() / 2;
    for (QGraphicsPixmapItem *snow : globular_snows_) {
        /// move down and left
        snow->moveBy(0, 20);
        snow->setOpacity(snow->opacity() + 0.005);

        /// move back up if too far down
        if (snow->y() > screen_bottom_y) {
            double offset_x = common_random_generator.rand_double(-300, maps_game->cam().width());
            double offset_y = common_random_generator.rand_double(0, maps_game->cam().height() * 2);
            snow->setPos(maps_game->cam().topLeft());
            snow->setY(snow->y() - snow->boundingRect().height() - offset_y);
            snow->setX(snow->x() + offset_x);
        }
    }
}

void SnowWeather::on_snow_step_linear() {
    Game *maps_game = map_->game();

    /// increase opacities
    if (snow1_->opacity() < 1) {
        snow1_->setOpacity(snow1_->opacity() + 0.005);
        snow2_->setOpacity(snow2_->opacity() + 0.005);
    }

    /// make sure both images are in the screen x wise
    snow1_->setX(maps_game->cam().topLeft().x());
    snow2_->setX(maps_game->cam().topLeft().x());

    /// move down both images
    snow1_->setY(snow1_->y() + 20);
    snow2_->setY(snow2_->y() + 20);

    /// if snow1 is at bottom, move it ontop of snow2
    double bottom_y = maps_game->cam().bottom();
    if (snow1_->y() > bottom_y) {
        snow1_->setY(snow2_->y() - snow1_->boundingRect().height());
    }

    /// if snow2 is at bottom, move it ontop of snow1
    if (snow2_->y() > bottom_y) {
        snow2_->setY(snow1_->y() - snow2_->boundingRect().height());
    }
}

void SnowWeather::start_timers() {
    globular_snow_timer_->start(100);
    linear_snow_timer_->start(100);
}
