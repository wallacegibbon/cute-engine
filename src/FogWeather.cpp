#include "FogWeather.h"
#include "Game.h"
#include "Map.h"
#include "Utilities.h"

using namespace cute;

FogWeather::FogWeather(QPixmap tileable_fog_graphic, int tile_width, int tile_height, double initial_opacity,
                       double final_opacity, double opacity_fade_time_ms, double opacity_step_size)
        : initial_opacity_(initial_opacity), max_opacity_(final_opacity), opacity_fade_time_(opacity_fade_time_ms),
          opacity_step_size_(opacity_step_size), fog_direction_(QVector2D(0, 1)), // down
          fog_picture_(tileable_fog_graphic), fog_picture_height_(tile_height), fog_picture_width_(tile_width) {

    opacity_timer_ = new QTimer(this);
    move_timer_ = new QTimer(this);
    connect(opacity_timer_, &QTimer::timeout, this, &FogWeather::on_opacity_step);
    connect(move_timer_, &QTimer::timeout, this, &FogWeather::on_move_step);

    current_opacity_ = initial_opacity_;
}

FogWeather::~FogWeather() {
    /// remove/delete all fog squares
    for (QGraphicsPixmapItem *fog_square : fog_squares_) {
        map_->scene()->removeItem(fog_square);
        delete fog_square;
    }
}

void FogWeather::start_() {
    Game *maps_game = map_->game();

    /// create fog squares
    int num_fog_squares_needed_x = maps_game->cam().width() / fog_picture_width_ + 3;
    int num_fog_squares_needed_y = maps_game->cam().height() / fog_picture_height_ + 3;
    for (int i = 0, n = num_fog_squares_needed_x; i < n; i++) {
        for (int j = 0, p = num_fog_squares_needed_y; j < p; j++) {
            fog_picture_ = fog_picture_.scaled(fog_picture_width_, fog_picture_height_);

            QGraphicsPixmapItem *fog_square = new QGraphicsPixmapItem(fog_picture_);
            fog_square->setOpacity(initial_opacity_);
            map_->scene()->addItem(fog_square);

            fog_squares_.insert(fog_square);
        }
    }

    /// set fog squares positions
    double fog_boundry_width = num_fog_squares_needed_x * fog_picture_width_;
    double fog_boundry_height = num_fog_squares_needed_y * fog_picture_height_;
    fog_boundry_ = QRectF(0, 0, fog_boundry_width, fog_boundry_height);
    fog_boundry_.moveCenter(maps_game->center_cam_pos());

    int cellx = 0;
    int celly = 0;
    for (QGraphicsPixmapItem *fog_square : fog_squares_) {
        double actualx = cellx * fog_picture_width_ + fog_boundry_.topLeft().x();
        double actualy = celly * fog_picture_height_ + fog_boundry_.topLeft().y();
        fog_square->setPos(actualx, actualy);
        cellx++;
        if (cellx >= num_fog_squares_needed_x) {
            cellx = 0;
            celly++;
        }
    }

    /// start timer to move fog squres
    start_timers();
}

void FogWeather::resume_() { start_timers(); }

void FogWeather::pause_() {
    opacity_timer_->stop();
    move_timer_->stop();
}

void FogWeather::set_fog_speed(double pixels_per_second) {
    fog_speed_ = pixels_per_second;
    pause_();
    resume_();
}

/// Sets the granularity of movement of the fog.
/// The lower, the smoother the movement but the more expensive.
void FogWeather::set_fog_step_size(double num_pixels) {
    fog_step_size_ = num_pixels;
    pause_();
    resume_();
}

void FogWeather::stop_() {
    opacity_timer_->stop();
    move_timer_->stop();

    /// remove all fog squares
    for (QGraphicsPixmapItem *fog_square : fog_squares_) {
        map_->scene()->removeItem(fog_square);
    }
}

void FogWeather::on_opacity_step() {
    /// increase opacities
    if (current_opacity_ < max_opacity_) {
        current_opacity_ += opacity_step_size_;
        for (QGraphicsPixmapItem *fog_square : fog_squares_) {
            fog_square->setOpacity(current_opacity_);
        }
    } else {
        opacity_timer_->disconnect();
    }
}

void FogWeather::on_move_step() {
    Game *maps_game = map_->game();

    /// move all fog squares down
    for (QGraphicsPixmapItem *fog_square : fog_squares_) {
        fog_square->moveBy(0, fog_step_size_);
    }

    /// set fog boundry based on location of cam
    fog_boundry_.moveCenter(maps_game->center_cam_pos());
    for (QGraphicsPixmapItem *fog_square : fog_squares_) {
        double fog_square_left = fog_square->x();
        double fog_square_right = fog_square->x() + fog_picture_width_;
        double fog_square_top = fog_square->y();
        double fog_square_bottom = fog_square->y() + fog_picture_height_;

        /// if the fog square is too far down, move it back up
        if (fog_square_top > fog_boundry_.bottom()) {
            double offset = fog_square_top - fog_boundry_.bottom();
            fog_square->setY(fog_boundry_.top() + offset);
        }
        /// if the fog square is too far up, move it back down
        if (fog_square_bottom < fog_boundry_.top()) {
            double offset = fog_boundry_.top() - fog_square_bottom;
            fog_square->setY(fog_boundry_.bottom() - fog_picture_height_ - offset);
        }
        /// if the fog square is too far left, move it to right
        if (fog_square_right < fog_boundry_.left()) {
            double offset = fog_boundry_.left() - fog_square_right;
            fog_square->setX(fog_boundry_.right() - fog_picture_width_ - offset);
        }
        /// if the fog square is too far right, move it to the left
        if (fog_square_left > fog_boundry_.right()) {
            double offset = fog_square_left - fog_boundry_.right();
            fog_square->setX(fog_boundry_.left() + offset);
        }
    }
}

void FogWeather::start_timers() {
    double opacity_rate = (max_opacity_ - initial_opacity_) / opacity_fade_time_; /// units per ms

    opacity_timer_->start(frequency(opacity_step_size_, opacity_rate));
    move_timer_->start(s_to_ms(frequency(fog_step_size_, fog_speed_)));
}
