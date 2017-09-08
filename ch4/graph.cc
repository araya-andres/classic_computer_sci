#include "prettyprint.hpp"
#include <algorithm>
#include <deque>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <utility>
#include <vector>

using Edge = std::pair<int, int>;
Edge reverse(const Edge& e) { return {e.second, e.first}; }

std::ostream& operator<<(std::ostream& os, const Edge& e) {
    return os << "(" << e.first << " <-> " << e.second << ")";
}

template <typename Vertex>
using Path = std::vector<Vertex>;

template <typename Vertex>
struct Graph {
    Graph(const std::vector<Vertex>& vertices) : vertices_(vertices) {}

    int add_vertex(const Vertex& v) {
        vertices_.push_back(v);
        return vertices_.size() - 1;
    }

    void add_edge(const Vertex& from, const Vertex& to) {
        auto i = index_of(from);
        auto j = index_of(to);
        if (i != -1 && j != -1) add_edge({i, j});
    }

    void add_edges(const std::vector<std::pair<Vertex, Vertex>>& edges) {
        for (const auto& e: edges) add_edge(e.first, e.second);
    }

    size_t vertex_count() const { return vertices_.size(); }

    size_t edge_count() const { return edges_.size(); }

    auto neighbors_for(const Vertex& v) const {
        return neighbors_for(index_of(v));
    }

    auto edges_for(const Vertex& v) const {
        return edges_for(index_of(v));
    }

    const std::vector<Vertex>& vertices() const { return vertices_; }

    Path<Vertex> bfs(const Vertex& v0, const std::function<bool(const Vertex&)>& goal_test) {
        const auto start_index = index_of(v0);
        if (start_index == -1) return {};
        std::deque<int> frontier{start_index};
        std::set<int> explored;
        std::map<int, Edge> path_dict;
        do {
            const auto& current_index = frontier.front();
            if (goal_test(vertex_at(current_index))) {
                return path_dict_to_path(start_index, current_index, path_dict);
            }
            for (const auto& e: edges_for(current_index)) {
                const auto to = e.second;
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

private:

    void add_edge(const Edge& e) {
        auto first = edges_.cbegin();
        auto last = edges_.cend();
        auto it = find(first, last, e);
        if (it == last) {
            edges_.push_back(e);
            edges_.push_back(reverse(e));
        }
    }

    Vertex vertex_at(int index) const {
        return vertices_.at(index);
    }

    int index_of(const Vertex& v) const {
        auto first = vertices_.cbegin();
        auto last = vertices_.cend();
        auto it = std::find(first, last, v);
        return it == last
            ? -1
            : std::distance(first, it);
    }

    auto neighbors_for(int index) const {
        std::vector<Vertex> neighbors;
        for (const auto& e: edges_) {
            if (e.first == index) {
                neighbors.push_back(vertices_[e.second]);
            }
        }
        return neighbors;
    }

    auto edges_for(int index) const {
        std::vector<Edge> edges;
        for (const auto& e : edges_) {
            if (e.first == index) edges.push_back(e);
        }
        return edges;
    }

    Path<Vertex> path_dict_to_path(const int start_index, const int current_index, const std::map<int, Edge>& dict) {
        std::vector<Vertex> tmp;
        for (auto from = dict.at(current_index).first; from != start_index; from = dict.at(from).first) {
            tmp.push_back(vertex_at(from));
        }
        return {tmp.crbegin(), tmp.crend()};
    }

    std::vector<Vertex> vertices_;
    std::vector<Edge> edges_;
};

template <typename Vertex>
std::ostream& operator<<(std::ostream& os, const Graph<Vertex>& g) {
    for (const auto& v: g.vertices()) {
        os << v << " -> " << g.neighbors_for(v) << '\n';
    }
    return os;
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
    Graph<const char*> g{{
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
