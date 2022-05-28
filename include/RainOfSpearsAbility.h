#pragma once

#include "NoTargetAbility.h"
#include "Vendor.h"

namespace cute {

class Sound;

/// An Ability that will rain spears around the owner, damaging all enemies of the owner but not the owner himself.
class RainOfSpearsAbility : public NoTargetAbility {
    Q_OBJECT

public:
    RainOfSpearsAbility(Entity *owner = nullptr);

    void use_implementation() override;

public slots:
    void on_spear_step();

private:
    int times_;
    QTimer *timer_;
    Sound *sound_effect_;
};

} // namespace cute
