#pragma once

#include "NoTargetItem.h"

class QTimer;

namespace cute {

class RainOfSpearsAbility;

class ItemRainOfSpears : public NoTargetItem {
public:
    ItemRainOfSpears();
    void use_() override;
};

} // namespace cute
