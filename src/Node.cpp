#include "Node.h"

using namespace cute;

bool cute::operator==(const Node &lhs, const Node &rhs) { return (lhs.x() == rhs.x()) && (lhs.y() == rhs.y()); }

bool cute::operator!=(const Node &lhs, const Node &rhs) { return !(cute::operator==(lhs, rhs)); }
