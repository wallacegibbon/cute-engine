#include "NoTargetAbility.h"
#include "Entity.h"

using namespace cute;

void NoTargetAbility::use() {
    assert_owner_is_alive();
    use_implementation();
}
