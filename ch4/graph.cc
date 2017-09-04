#include "prettyprint.hpp"
#include <iostream>
#include <utility>
#include <vector>

using Edge = std::pair<int, int>;
Edge reverse(const Edge& e) { return {e.second, e.first}; }

std::ostream& operator<<(std::ostream& os, const Edge& e) {
    return os << "(" << e.first << "<->" << e.second << ")";
}

using UnweightedEdge = Edge;

template <typename Vertex>
struct Graph {
    int add_vertex(const Vertex& v) {
        vertices.push_back(v);
        return vertices.size() - 1;
    }

    void add_edge(const Edge& e) {
        auto first = edges.cbegin();
        auto last = edges.cend();
        auto it = find(first, last, e);
        if (it != last) {
            edges.push_back(e);
            edges.push_back(reverse(e));
        }
    }

    size_t vertex_count() const { return vertices.size(); }

    size_t edge_count() const { return edges.size(); }

    Vertex vertex_at(int index) const {
        return vertices.at(index);
    }

    int index_of(const Vertex& v) const {
        auto first = vertices.cbegin();
        auto last = vertices.cend();
        auto it = find_if(first, last, v);
        return it == last
            ? -1
            : std::distance(first, it);
    }

    std::vector<int> neighbors_for_index(int index) const {
        std::vector<int> neighbors;
        std::transform(vertices.cbegin(), vertices.cend(),
                std::back_inserter(neighbors),
                [=](const Edge& e) { return e.first == index; });
        return neighbors;
    }

    std::vector<int> neighbors_for_vertex(const Vertex& v) const {
        return neighbors_for_index(index_of(v));
    }

    std::vector<Edge> edges_for_index(int index) const {
        std::vector<Edge> edges;
        std::transform(edges.cbegin(), edges.cend(),
                std::back_inserter(edges),
                [=](const Edge& e) { return e.first == index; });
        return edges;
    }

    std::vector<int> edges_for_vertex(const Vertex& v) const {
        return edges_for_index(index_of(v));
    }

private:
    std::vector<Vertex> vertices;
    std::vector<Edge> edges;
};

int main() {
    Edge e{1,2};
    std::cout << e << '\n';
    std::cout << reverse(e) << '\n';
}
