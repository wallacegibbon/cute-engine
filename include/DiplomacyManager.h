#pragma once

#include "Vendor.h"

namespace cute {

enum class Relationship { FRIEND, NEUTRAL, ENEMY, UNSPECIFIED };

class DiplomacyManager {
public:
    Relationship get_relationship(int group1, int group2);
    void set_relationship(int group1, int group2, Relationship relationship);

    class Hasher {
    public:
        size_t operator()(const std::pair<int, int> &pair) const {
            /// TODO: improve hash algorithm, also consider making a simple reusable point hash algorithm
            return std::hash<int>()(pair.first) + std::hash<int>()(pair.second);
        }
    };

private:
    /// A treat B as enemy doesn't mean B treat A as enemy
    std::unordered_map<std::pair<int, int>, Relationship, Hasher> pair_to_relationship_;
};

} // namespace cute
