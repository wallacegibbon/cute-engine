#pragma once

#include "NoTargetAbility.h"
#include "Vendor.h"

namespace cute {

class Sound;

class ShardsOfFireAbility : public NoTargetAbility {
public:
    ShardsOfFireAbility(int num_shards, double shard_distance, Entity *owner = nullptr);
    void use_implementation() override;

private:
    Sound *sound_effect_;
    int num_shards_;
    double shard_distance_;
};

} // namespace cute
