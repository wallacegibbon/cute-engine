#include "RandomGenerator.h"

namespace cute {
RandomGenerator common_random_generator;
}

using namespace cute;

RandomGenerator::RandomGenerator() { srand(time(0)); }

int RandomGenerator::rand_int(int min, int max) {
    int range = max - min + 1;
    return rand() % range + min;
}

int RandomGenerator::rand_double(double min, double max) {
    double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    return min + (max - min) * r;
}

QPointF RandomGenerator::rand_QPointF(const QPointF &top_left, const QPointF &bottom_right) {
    double rx = rand_double(top_left.x(), bottom_right.x());
    double ry = rand_double(top_left.y(), bottom_right.y());
    return QPointF(rx, ry);
}

QPointF RandomGenerator::rand_QPointF(const QRectF &in_region) {
    return rand_QPointF(in_region.topLeft(), in_region.bottomRight());
}
