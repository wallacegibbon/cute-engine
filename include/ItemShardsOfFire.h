#pragma once

#include "NoTargetItem.h"

namespace cute {

class ShardsOfFireAbility;

class ItemShardsOfFire : public NoTargetItem {
public:
    ItemShardsOfFire(int num_shards = 10, double shard_distance = 600);
    void use_() override;
};

} // namespace cute
