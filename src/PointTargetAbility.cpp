#include "PointTargetAbility.h"

using namespace cute;

void PointTargetAbility::use(const QPointF &at_point) {
    assert_owner_is_alive();
    use_implementation(at_point);
}
