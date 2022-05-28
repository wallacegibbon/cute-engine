#pragma once

#include "Entity.h"
#include "EntityController.h"
#include "Vendor.h"

namespace cute {

class ECKeyboardMoverPerspective : public EntityController {
    Q_OBJECT

public:
    ECKeyboardMoverPerspective(Entity *entity);

    void set_step_size(double step_size) { step_size_ = step_size; }
    double step_size() { return step_size_; }

public slots:
    void on_move_step();

private:
    void step_on_relative_angle(int angle);
    void play_animation_if_no_other_playing(std::string anim);

private:
    double step_size_ = 16;
    QTimer *move_timer_;
};

} // namespace cute
