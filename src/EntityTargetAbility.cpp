#include "EntityTargetAbility.h"

using namespace cute;

void EntityTargetAbility::use(Entity &on_entity) {
    assert_owner_is_alive();
    use_implementation(on_entity);
}
