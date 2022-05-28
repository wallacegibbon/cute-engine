#include "Tree.h"

using namespace cute;

Tree::Tree(const Graph &graph, const Node &root) : graph_(graph), root_(root) {}

Tree::Tree(const Node &root) : root_(root), graph_() { graph_.add_node(root); }

void Tree::add_child(const Node &to, const Node &child, int weight) {
    /// make sure the to Node exists
    assert(graph_.contains(to));

    /// add child to graph
    graph_.add_node(child);

    /// add edge b/w the "to" node and the child
    graph_.add_edge(to, child, weight);
}

/// Returns the vector of Nodes that goes from the root Node to the specified Node.
std::vector<Node> Tree::path_to(const Node &node) const {
    /// initialze variables
    visited_nodes_.clear();

    std::vector<Node> empty;
    return dfs(root_, node, empty);
}

std::unordered_set<Node> Tree::nodes() const { return graph_.nodes(); }

std::unordered_set<Edge> Tree::edges() const { return graph_.edges(); }

/// Recursive dfs. Used by Tree::path_to(const Node&).
std::vector<Node> Tree::dfs(const Node &node, const Node &target, std::vector<Node> path) const {
    /// mark node visited & add it to the vector
    visit(node);

    /// if node == target, return path so far
    if (node == target) {
        path.push_back(node);
        return path;
    }

    /// if node doesn't have any unvisited children, remove it, and run dfs on previous node in path
    if (!has_unvisited_child(node)) {
        Node last_node = path.back();
        path.pop_back();
        return dfs(last_node, target, path);
    }

    /// if node has unvisited children, pick one and run dfs on it
    if (has_unvisited_child(node)) {
        path.push_back(node);
        Node unvisited = an_unvisited_child(node);
        return dfs(unvisited, target, path);
    }

    return path;
}

/// Marks the Node as visited (adds the Node to the set of visited Nodes).
/// The Tree maintains a set of visited Nodes as a helper attribute to dfs.
void Tree::visit(const Node &node) const {
    assert(graph_.contains(node));
    visited_nodes_.insert(node);
}

bool Tree::is_visited(const Node &node) const { return (visited_nodes_.count(node) == 1); }

bool Tree::has_unvisited_child(const Node &node) const {
    std::vector<Node> children = graph_.outgoing_nodes(node);
    int num_unvisited = 0;
    for (auto child : children) {
        if (!is_visited(child)) {
            num_unvisited++;
        }
    }
    return num_unvisited > 0;
}

Node Tree::an_unvisited_child(const Node &node) const {
    assert(has_unvisited_child(node));
    std::vector<Node> children = graph_.outgoing_nodes(node);
    for (auto child : children) {
        if (!is_visited(child)) {
            return child;
        }
    }
    assert(false);
    /// return something to fix the compiling warning
    return Node();
}
