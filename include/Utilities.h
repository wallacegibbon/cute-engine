#pragma once

#include "Vendor.h"

class QPointF;

namespace cute {

class Map;
class AIEntity;
class Entity;

void add_random_trees(Map *map, int nums_to_add, int num_images);
void add_random_tree(Map *map, int num_images);
void add_tag(const std::string &tag, std::initializer_list<Entity *> entities);

Entity *get_minotaur_entity();
Entity *get_spider_entity();

inline double s_to_ms(double seconds) { return seconds * 1000.0; }
inline double ms_to_s(double milliseconds) { return milliseconds / 1000.0; }

/// Given a step size and a target rate, will tell you the frequency needed to get the target rate.
/// I.e. if my character moves 10 pixels at a time (step_size) and I want him to move
/// 50 pixels per second (targetRate), how often (in seconds) should I move him?
/// I.e. if my character rotates 10 degrees at a time (step_size) and I want him to
/// rotate at 50 degrees per second, how often (in seconds) should I move him?
/// Obviously, you need to make sure you are consistent in your units.
inline double frequency(double step_size, double rate) { return step_size / rate; }

quint32 fast_random();

double closest_angle(const std::vector<int> &all_angles, double target_angle);

int rand_int(int from, int to);
double rand_double(double from, double to);

/// For integer division, C/C++ is round towards zero, this function just do floor operation like Python
/// For more information about integer division: <https://en.wikipedia.org/wiki/Modulo_operation>
template <typename T>
inline T floor_divide(T num, T gap) {
    if (num < 0) {
        return num / gap - 1;
    } else {
        return num / gap;
    }
}

} // namespace cute
