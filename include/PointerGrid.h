#pragma once

#include "Node.h"
#include "Vendor.h"
#include "stl_helper.h"

namespace cute {

template <typename PointerType>
class PointerGrid {
public:
    PointerGrid(int columns = 0, int rows = 0);

    void set_pointer_at(int x, int y, PointerType *pointer);
    void set_pointer_at(const Node &pos, PointerType *pointer);

    PointerType *pointer_at(int x, int y) const;
    bool contains(PointerType *pointer) const;
    Node position_of(PointerType *pointer) const;
    std::vector<PointerType *> non_null_pointers() const;

private:
    std::vector<PointerType *> pointers_;
    int columns_;
    int rows_;
};

template <typename PointerType>
PointerGrid<PointerType>::PointerGrid(int columns, int rows) : columns_(columns), rows_(rows) {
    for (int i = 0, n = columns * rows; i < n; i++) {
        pointers_.push_back(nullptr);
    }
}

template <typename PointerType>
void PointerGrid<PointerType>::set_pointer_at(int x, int y, PointerType *pointer) {
    assert(x < columns_ && y < rows_ && x >= 0 && y >= 0);
    pointers_[x + columns_ * y] = pointer;
}

template <typename PointerType>
void PointerGrid<PointerType>::set_pointer_at(const Node &pos, PointerType *pointer) {
    set_pointer_at(pos.x(), pos.y(), pointer);
}

template <typename PointerType>
PointerType *PointerGrid<PointerType>::pointer_at(int x, int y) const {
    if (x < 0 || x >= columns_ || y < 0 || y >= rows_) {
        return nullptr;
    }
    return pointers_[x + columns_ * y];
}

template <typename PointerType>
bool PointerGrid<PointerType>::contains(PointerType *pointer) const {
    for (PointerType *p : pointers_) {
        if (p == pointer) {
            return true;
        }
    }
    return false;
}

template <typename PointerType>
Node PointerGrid<PointerType>::position_of(PointerType *pointer) const {
    int index = stl_helper::find_index(pointers_, pointer);
    assert(index != -1);
    return Node(index % columns_, index / columns_);
}

template <typename PointerType>
std::vector<PointerType *> PointerGrid<PointerType>::non_null_pointers() const {
    std::vector<PointerType *> result;
    for (PointerType *p : pointers_) {
        if (p != nullptr) {
            result.push_back(p);
        }
    }
    return result;
}

} // namespace cute
