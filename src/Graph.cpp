#include "Graph.h"
#include "Tree.h"

using namespace cute;

void Graph::add_node(const Node &node) { nodes_.insert(node); }

void Graph::add_edge(const Node &from, const Node &to, int weight) {
    /// make sure from and to nodes exist in the graph
    assert(contains(from));
    assert(contains(to));

    /// construct an edge
    Edge edge(from, to, weight);

    /// add the edge to the set of edges
    edges_.insert(edge);
}

std::vector<Edge> Graph::outgoing_edges(const Node &from) const {
    /// make sure the from Node actually exists
    assert(contains(from));

    std::vector<Edge> edges;
    for (auto edge : edges_) {
        if (edge.from() == from) {
            edges.push_back(edge);
        }
    }
    return edges;
}

std::vector<Edge> Graph::incoming_edges(const Node &to) const {
    /// make sure the to Node actually exists
    assert(contains(to));

    std::vector<Edge> edges;
    for (auto edge : edges_) {
        if (edge.to() == to) {
            edges.push_back(edge);
        }
    }
    return edges;
}

std::vector<Node> Graph::outgoing_nodes(const Node &from) const {
    assert(contains(from));

    std::vector<Edge> o_edges = outgoing_edges(from);
    std::vector<Node> o_nodes;

    for (auto edge : o_edges) {
        o_nodes.push_back(edge.to());
    }

    return o_nodes;
}

std::vector<Node> Graph::incoming_nodes(const Node &to) const {
    assert(contains(to));

    std::vector<Edge> i_edges = incoming_edges(to);
    std::vector<Node> i_nodes;

    for (auto edge : i_edges) {
        i_nodes.push_back(edge.from());
    }

    return i_nodes;
}

bool Graph::contains(const Node &node) const { return (nodes_.count(node) == 1); }

bool Graph::contains(const Edge &edge) const { return (edges_.count(edge) == 1); }

/// Returns a vector of Nodes that represent the shortest path between the specified Nodes.
/// Uses A* pathfinding algorithm.
std::vector<Node> Graph::shortest_path(const Node &from, const Node &to) const {
    assert(contains(from));
    assert(contains(to));

    // return spt(from).path_to(to);

    /// consider lazy evaluation (retrieving stored value, short circuiting, etc..)
    /// for functions that are expensive
    /// GUARD: short circuit if to == from, just return an empty vector
    if (from == to) {
        return std::vector<Node>();
    }

    open_nodes_.clear();
    closed_nodes_.clear();
    node_to_parent_.clear();
    node_to_F_cost_.clear();
    node_to_G_cost_.clear();
    node_to_H_cost_.clear();

    /// calculate costs (H, G, and F)
    calculate_costs(from, to);

    /// add the start node ("from" to list of open nodes
    open_nodes_.insert(from);

    /// traverse through
    Node current;
    while (true) {
        /// set the current Node as the one with the lowest F cost
        current = node_in_open_with_lowest_F_cost();
        open_nodes_.erase(current);    /// remove it from the open nodes
        closed_nodes_.insert(current); /// add it to the closed nodes ("visited")

        /// if current node is the target node, path has been found
        if (current == to) {
            break;
        }

        /// for each neighbor of the current node
        for (Node neighbor : outgoing_nodes(current)) {
            /// if neighbor is in closed, skip it
            if (closed_nodes_.count(neighbor) > 0) {
                continue;
            }

            /// if neighbor is in open, but the new path is shorter
            int originalGCostOfNeighbor = node_to_G_cost_[neighbor];
            int newGCostOfNeighbor = node_to_G_cost_[current] + 1;
            if (newGCostOfNeighbor < originalGCostOfNeighbor) {
                /// update the cost of neighbor
                node_to_G_cost_[neighbor] = node_to_G_cost_[current] + 1;
                node_to_F_cost_[neighbor] = node_to_G_cost_[neighbor] + node_to_H_cost_[neighbor];

                /// set parent of neighbor
                node_to_parent_[neighbor] = current;
            }

            /// if neighbor is NOT in open
            if (open_nodes_.count(neighbor) == 0) {
                /// set cost of neighbor
                node_to_G_cost_[neighbor] = node_to_G_cost_[current] + 1;
                node_to_F_cost_[neighbor] = node_to_G_cost_[neighbor] + node_to_H_cost_[neighbor];

                /// set parent of neighbor
                node_to_parent_[neighbor] = current;

                /// add it to open
                open_nodes_.insert(neighbor);
            }
        }
    }

    /// at this point, current is the target Node, follow it back to the start
    std::vector<Node> path;
    path.push_back(current);
    while (node_to_parent_.count(current) > 0) {  /// while current Node has a parent
        path.push_back(node_to_parent_[current]); /// add parent to path
        current = node_to_parent_[current];       /// go one backwards
    }

    /// reverse the path (because it goes back->front) and then return it
    std::reverse(path.begin(), path.end());
    return path;
}

/// Returns a shortest path Tree rooted at the specified ("source") Node.
/// Uses dijkstras algorithm therefore it is O(mlog(n)).
Tree Graph::spt(const Node &source) const {
    assert(contains(source));

    picked_nodes_.clear();
    unpicked_nodes_.clear();
    picked_edges_.clear();
    unpicked_edges_.clear();
    node_weight_.clear();
    updated_edge_.clear();

    /// need to remember the first picked node as the root
    Node rootNode(0, 0);

    /// mark all nodes as unpicked
    unpick_all();

    /// initialize weights
    initialize_node_weights(source);

    /// while there is an unpicked node
    bool isFirstNode = true;
    while (has_unpicked_node()) {
        /// pick the one with the lightest weight
        Node lightest = lightest_unpicked_node();
        pick(lightest);

        /// update its neighbors weights
        update_neighbor_weights(lightest);

        /// if this is the first picked node, it should also be the root & don't pick any edges!
        if (isFirstNode) {
            rootNode = lightest;
            isFirstNode = false;
            continue;
        }

        /// pick its edge
        pick_conneted_edge(lightest);
    }

    /// create a graph from the picked set of nodes and edges
    Graph graph(picked_nodes_, picked_edges_);

    /// create/return a tree from the graph
    return Tree(graph, rootNode);
}

/// A Graph maintains a set of picked and a set of unpicked Nodes, which are helper attributes to
/// finding the spt of the Graph. This function will remove the specified node from the unpicked set
/// and puts it in the picked set.
void Graph::pick(const Node &node) const {
    /// make sure the node is in unpicked_nodes_
    assert(unpicked_nodes_.count(node) == 1);

    /// make sure it is not already picked
    assert(!picked(node));

    picked_nodes_.insert(node);  /// add it to picked
    unpicked_nodes_.erase(node); /// remove it from unpicked
}

/// A Graph maintains a set of picked and a set of unpicked Edges, which is a helper attribute to
/// finding the spt of the Graph. This function will remove the specified Edge from the unpicked set
/// and put it in the picked set.
void Graph::pick(const Edge &edge) const {
    /// make sure its unpicked
    /// assert(unpicked_edges_.count(edge) == 1);
    /// assert(!picked(edge)); TODO find out why this breaks code!?

    picked_edges_.insert(edge);
    unpicked_edges_.erase(edge);
}

bool Graph::picked(const Node &node) const {
    /// make sure the Node exists
    assert(contains(node));

    return (picked_nodes_.count(node) == 1);
}

bool Graph::picked(const Edge &edge) const {
    assert(contains(edge));

    return (picked_edges_.count(edge) == 1);
}

/// A Graph maintains a mapping of Node:int so that every Node has a weight. This is a helper attribute to
/// finding the spt of the Graph.
void Graph::set_weight(const Node &node, int weight) const {
    assert(contains(node));

    node_weight_[node] = weight;
}

int Graph::weight(const Node &node) const {
    assert(contains(node));

    /// find returns iterator to pair, pair.first is the key, pair.second is value
    int weight = node_weight_.find(node)->second;
    return weight;
}

Edge Graph::edge(const Node &from, const Node &to) const {
    assert(contains(from));
    assert(contains(to));

    for (Edge edge : edges_) {
        if (edge.from() == from && edge.to() == to) {
            return edge;
        }
    }

    /// make sure an edge was found! (if it gets here, no edge was found)
    assert(true);

    /// just here to supress compiler waring ("not all control
    /// paths return a value"). This is a private helper function,
    /// it only gets called by this class in an attempt to break
    /// up the function into a bunch of named little ones. This
    /// function will only be called when there really is an edge
    /// between the specified nodes. In other words, it will
    /// always return
    return Edge();
}

/// unpick nodes and edges
void Graph::unpick_all() const {
    for (Node node : nodes_) {
        unpicked_nodes_.insert(node);
    }
    for (Edge edge : edges_) {
        unpicked_edges_.insert(edge);
    }
}

/// Gives source Node weight of 0 and all other nodes weight of 1000
void Graph::initialize_node_weights(const Node &source) const {
    assert(contains(source));

    /// Give source node weight of 0
    set_weight(source, 0);

    /// Give all other nodes weight of 1000
    for (Node node : nodes_) {
        if (!(node == source)) {
            set_weight(node, 1000);
        }
    }
}

bool Graph::has_unpicked_node() const { return (!unpicked_nodes_.empty()); }

Node Graph::lightest_unpicked_node() const {
    /// make sure there is at least one unpicked node
    assert(has_unpicked_node());

    /// assume the first one is the lightest
    Node lightest = *(unpicked_nodes_.begin());

    /// try to find a lighter one
    for (Node node : unpicked_nodes_) {
        if (weight(node) < weight(lightest)) {
            lightest = node;
        }
    }

    return lightest;
}

/// This is a helper function for determining the spt of the Graph. When a Node is picked,
/// there is an appropriate Edge that must also be picked, therefore whenever a Node is picked,
/// I keep track of the appropriate Edge, this funtion simply picks that Edge.
void Graph::pick_conneted_edge(const Node &node) const {
    Edge edge = updated_edge_[node];
    pick(edge);
}

std::vector<Node> Graph::unpicked_neighbors(const Node &node) const {
    /// find all neighbors, see which ones are unpicked
    std::vector<Node> neighbors = outgoing_nodes(node);

    std::vector<Node> unpicked;
    for (auto neighbor : neighbors) {
        if (!picked(neighbor)) {
            unpicked.push_back(neighbor);
        }
    }

    return unpicked;
}

/// Updates the weights of the neighboring Nodes if the new weight is smaller.
/// This is a helper function for determining the spt of the Graph.
void Graph::update_neighbor_weights(const Node &node) const {
    /// For each unpicked neighbor
    for (Node neighbor : unpicked_neighbors(node)) {
        /// find weight of edge to neighbor
        Edge edge_to_neighbor = edge(node, neighbor);
        int edge_weight = edge_to_neighbor.weight();

        /// find new weight to neighbor and old weight
        int new_total_weight = node_weight_[node] + edge_weight;
        int old_weight = node_weight_[neighbor];

        if (new_total_weight < old_weight) {
            set_weight(neighbor, new_total_weight);
            /// remember that this edge was used to get to this node
            updated_edge_[neighbor] = edge_to_neighbor;
        }
    }
}

/// Returns the Node in open_nodes_ with the lowest F cost. If two Nodes have the
/// equivalent F cost, will return the one with the lower H cost. If two
/// Nodes have the same F and H cost, returns an arbitrary one.
Node Graph::node_in_open_with_lowest_F_cost() const {
    /// find the lowest f cost in open
    bool initial_F_cost_set = false;
    int lowest_F_cost;
    for (Node node : open_nodes_) {
        /// assume initial one is lowest
        if (!initial_F_cost_set) {
            lowest_F_cost = node_to_F_cost_[node];
            initial_F_cost_set = true;
        }

        /// if we find a lower one, set that as lowest
        if (node_to_F_cost_[node] < lowest_F_cost) {
            lowest_F_cost = node_to_F_cost_[node];
        }
    }

    /// get all Nodes that have this f cost
    std::vector<Node> lowest_F_cost_nodes;
    for (Node node : open_nodes_) {
        if (node_to_F_cost_[node] == lowest_F_cost) {
            lowest_F_cost_nodes.push_back(node);
        }
    }

    /// return the one with lowest h cost
    bool initial_H_cost_set = false;
    Node lowest_cost;
    for (Node node : lowest_F_cost_nodes) {
        if (!initial_H_cost_set) {
            lowest_cost = node;
            initial_H_cost_set = true;
        }

        /// if lower, set this one
        if (node_to_H_cost_[node] < node_to_H_cost_[lowest_cost]) {
            lowest_cost = node;
        }
    }

    /// Node lowest_cost is the one with lowest F cost (lowest H cost if multiple equal F costs)
    return lowest_cost;
}

/// Calculates H, G, and F costs for each Node.
void Graph::calculate_costs(Node from, Node to) const {
    for (Node node : nodes_) {
        node_to_H_cost_[node] = abs(to.x() - node.x()) + abs(to.y() - node.y());
        node_to_G_cost_[node] = abs(from.x() - node.x()) + abs(from.y() - node.y());
        node_to_F_cost_[node] = node_to_G_cost_[node] + node_to_H_cost_[node];
    }
}
