#pragma once

#include "Entity.h"
#include "EntityController.h"
#include "Vendor.h"

class QTimer;

namespace cute {

class ECRotater;

class ECMouseFacer : public EntityController {
    Q_OBJECT

public:
    ECMouseFacer(Entity *entity);

public slots:
    void on_rotate_step();

private:
    double rotate_step_size_ = 5;  /// in degrees
    double rotate_frequency_ = 30; /// in ms
    QTimer *rotate_timer_;
    ECRotater *rotater_;
};

} // namespace cute
