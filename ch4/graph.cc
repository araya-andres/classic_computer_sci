#include "prettyprint.hpp"
#include <algorithm>
#include <deque>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <utility>
#include <vector>

struct UnweightedEdge { int u, v; };

UnweightedEdge reverse(const UnweightedEdge& e) { return {e.v, e.u}; }

std::ostream& operator<<(std::ostream& os, const UnweightedEdge& e) {
    return os << "{" << e.u << " <-> " << e.v << "}";
}

bool operator==(const UnweightedEdge& lhs, const UnweightedEdge& rhs) {
    return lhs.u == rhs.u && lhs.v == rhs.v;
}

bool operator<(const UnweightedEdge& lhs, const UnweightedEdge& rhs) {
    return (lhs.u < rhs.u)
        || (lhs.u == rhs.u && lhs.v < rhs.v);
}

template <typename Weight>
struct WeightedEdge {
    int u, v;
    Weight weight;
};

template<typename Weight>
WeightedEdge<Weight> reverse(const WeightedEdge<Weight>& e) { return {e.v, e.u, e.weight}; }

template <typename Weight>
bool operator==(const WeightedEdge<Weight>& lhs, const WeightedEdge<Weight>& rhs) {
    return lhs.u == rhs.u && lhs.v == rhs.v && lhs.weight == rhs.weight;
}

template <typename Weight>
bool operator<(const WeightedEdge<Weight>& lhs, const WeightedEdge<Weight>& rhs) {
    return lhs.u == rhs.u && lhs.v == rhs.v && lhs.weight < rhs.weight;
}

template <typename Weight>
std::ostream& operator<<(std::ostream& os, const WeightedEdge<Weight>& e) {
    return os << "{" << e.u << " <-(" << e.weight << ")-> " << e.v << "}";
}

template <typename Vertex>
using Path = std::vector<Vertex>;

template <typename Vertex, typename Edge = UnweightedEdge>
struct Graph {
    Graph(const std::vector<Vertex>& vertices) : vertices_(vertices) {}

    // add a vertex to the graph
    int add_vertex(const Vertex& v) {
        vertices_.push_back(v);
        return vertices_.size() - 1;
    }

    // add an edge to the graph
    void add_edge(const Edge& e) {
        auto first = edges_.cbegin();
        auto last = edges_.cend();
        auto it = find(first, last, e);
        if (it == last) {
            edges_.push_back(e);
            edges_.push_back(reverse(e));
        }
    }

    // how many vertices are in the graph?
    size_t vertex_count() const { return vertices_.size(); }

    // how many edges are in the graph?
    size_t edge_count() const { return edges_.size(); }

    auto neighbors_for(const Vertex& v) const {
        return neighbors_for(index_of(v));
    }

    auto edges_for(const Vertex& v) const {
        return edges_for(index_of(v));
    }

    // get vertex by index
    Vertex vertex_at(int index) const {
        return vertices_.at(index);
    }

    // find the first occurence of a vertex if it exists
    int index_of(const Vertex& v) const {
        auto u = vertices_.cbegin();
        auto last = vertices_.cend();
        auto it = std::find(u, last, v);
        return it == last
            ? -1
            : std::distance(u, it);
    }

    // find all neighbors of a vertex at a given index
    auto neighbors_for(int index) const {
        std::vector<Vertex> neighbors;
        for (const auto& e: edges_) {
            if (e.u == index) {
                neighbors.push_back(vertices_[e.v]);
            }
        }
        return neighbors;
    }

    // find all vertex at the given index
    auto edges_for(int index) const {
        std::vector<Edge> edges;
        for (const auto& e : edges_) {
            if (e.u == index) edges.push_back(e);
        }
        return edges;
    }

    Path<Vertex> path_dict_to_path(const int start_index, const int current_index, const std::map<int, Edge>& dict) {
        std::vector<Vertex> tmp{vertex_at(current_index)};
        for (auto from = dict.at(current_index).u; from != start_index; from = dict.at(from).u) {
            tmp.push_back(vertex_at(from));
        }
        tmp.push_back(vertex_at(start_index));
        return {tmp.crbegin(), tmp.crend()};
    }

    std::string to_str() const {
        std::ostringstream oss;
        for (const auto& v: vertices_) {
            oss << v << " -> " << neighbors_for(v) << '\n';
        }
        return oss.str();
    }

private:

    std::vector<Vertex> vertices_;
    std::vector<Edge> edges_;
};

template<typename Vertex>
struct UnweightedGraph {
    UnweightedGraph(const std::vector<Vertex>& vertices) : g(vertices) {}

    void add_edge(const Vertex& from, const Vertex& to) {
        auto i = g.index_of(from);
        auto j = g.index_of(to);
        if (i != -1 && j != -1) g.add_edge({i, j});
    }

    void add_edges(const std::vector<std::pair<Vertex, Vertex>>& edges) {
        for (const auto& e: edges) add_edge(e.first, e.second);
    }

    Path<Vertex> bfs(const Vertex& v0, const std::function<bool(const Vertex&)>& goal_test) {
        const auto start_index = g.index_of(v0);
        if (start_index == -1) return {};
        std::deque<int> frontier{start_index};
        std::set<int> explored;
        std::map<int, UnweightedEdge> path_dict;
        do {
            const auto& current_index = frontier.front();
            if (goal_test(g.vertex_at(current_index))) {
                return g.path_dict_to_path(start_index, current_index, path_dict);
            }
            for (const auto& e: g.edges_for(current_index)) {
                const auto to = e.v;
                if (explored.find(to) == explored.end()) {
                    explored.insert(to);
                    frontier.push_back(to);
                    path_dict[to] = e;
                }
            }
            frontier.pop_front();
        } while (!frontier.empty());
        return {};
    }

    std::string to_str() const { return g.to_str(); }

private:
    Graph<Vertex, UnweightedEdge> g;
};

template <typename Vertex>
std::ostream& operator<<(std::ostream& os, const UnweightedGraph<Vertex>& g) {
    return os << g.to_str();
}

const char* atlanta = "Atlanta";
const char* boston = "Boston";
const char* chicago = "Chicago";
const char* dallas = "Dallas";
const char* detroit = "Detroit";
const char* houston = "Houston";
const char* los_angeles = "Los Angeles";
const char* miami = "Miami";
const char* new_york = "New York";
const char* philadelphia = "Philadelphia";
const char* phoenix = "Phoenix";
const char* riverside = "Riverside";
const char* san_francisco = "San Francisco";
const char* seattle = "Seattle";
const char* washington = "Washington";

int main() {
    UnweightedGraph<const char*> g{{
        atlanta,
        boston,
        chicago,
        dallas,
        detroit,
        houston,
        los_angeles,
        miami,
        new_york,
        philadelphia,
        phoenix,
        riverside,
        san_francisco,
        seattle,
        washington,
    }};
    g.add_edges({
        {seattle, chicago},
        {seattle, san_francisco},
        {san_francisco, riverside},
        {san_francisco, los_angeles},
        {los_angeles, riverside},
        {los_angeles, phoenix},
        {riverside, phoenix},
        {riverside, chicago},
        {phoenix, dallas},
        {phoenix, houston},
        {dallas, chicago},
        {dallas, atlanta},
        {dallas, houston},
        {houston, atlanta},
        {houston, miami},
        {atlanta, chicago},
        {atlanta, washington},
        {atlanta, miami},
        {miami, washington},
        {chicago, detroit},
        {detroit, boston},
        {detroit, washington},
        {detroit, new_york},
        {boston, new_york},
        {new_york, philadelphia},
        {philadelphia, washington},
    });
    std::cout << g << '\n';
    auto boston_to_miami = g.bfs(boston, [](const char* dest) { return dest == miami; });
    std::cout << "Boston to Miami: " << boston_to_miami << '\n';
}
