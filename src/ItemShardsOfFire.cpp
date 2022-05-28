#include "ItemShardsOfFire.h"
#include "ShardsOfFireAbility.h"

cute::ItemShardsOfFire::ItemShardsOfFire(int num_shards, double shard_distance) {
    set_description("Spits shards of fire around the owner.");
    set_ability(new ShardsOfFireAbility(num_shards, shard_distance));
}

void cute::ItemShardsOfFire::use_() {
    /// do nothing (item has ability, thus ability will be used on use())
}
