#include "DiplomacyManager.h"

using namespace cute;

Relationship DiplomacyManager::get_relationship(int group1, int group2) {
    auto iter = pair_to_relationship_.find(std::make_pair(group1, group2));
    if (iter == pair_to_relationship_.end()) {
        return Relationship::UNSPECIFIED;
    }
    return (*iter).second;
}

void DiplomacyManager::set_relationship(int group1, int group2, Relationship relationship) {
    pair_to_relationship_[std::make_pair(group1, group2)] = relationship;
}
