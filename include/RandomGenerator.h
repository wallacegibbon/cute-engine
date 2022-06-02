#pragma once

#include "Vendor.h"

class QPointF;
class QRectF;

namespace cute {

class RandomGenerator {
public:
    RandomGenerator();

    int rand_int(int min, int max);
    int rand_double(double start, double end);
    QPointF rand_QPointF(const QPointF &top_left, const QPointF &bottom_right);
    QPointF rand_QPointF(const QRectF &in_region);
};

extern RandomGenerator common_random_generator;

} // namespace cute
