#include <iostream>
#include <vector>
#include <climits>
#include <deque>
#include <map>
#include "../algo_and_ds/Heap.hpp"

struct Edge {
    int from, to;

    explicit Edge(int from = 0, int to = 0) : from(from), to(to) {}
};

struct WeightedEdge : public Edge {
    int weight;

    explicit WeightedEdge(int from = 0, int to = 0, int weight = 0) : Edge(from, to), weight(weight) {}

    bool operator<(const WeightedEdge& other) const { return weight < other.weight; }

    bool operator>(const WeightedEdge& other) const { return weight > other.weight; }

    bool operator==(const WeightedEdge& other) const { return weight == other.weight; }

    bool operator!=(const WeightedEdge& other) const { return weight != other.weight; }

    bool operator<=(const WeightedEdge& other) const { return weight <= other.weight; }

    bool operator>=(const WeightedEdge& other) const { return weight >= other.weight; }
};

class Graph {
private:
    std::vector<std::vector<WeightedEdge>> edge_list;
    std::vector<std::vector<WeightedEdge>> reverse_edge_list;
public:
    Graph() {}

    int nodeCount() const { return edge_list.size(); }

    std::vector<WeightedEdge> const& edges(int from) const { return edge_list[from]; }

    std::vector<WeightedEdge> const& reverse_edges(int to) const { return reverse_edge_list[to]; }

    friend std::istream& operator>>(std::istream& is, Graph& g);
};

std::istream& operator>>(std::istream& is, Graph& g) {
    int node_count;
    is >> node_count;
    g.edge_list.resize(node_count);
    g.reverse_edge_list.resize(node_count);
    for (int from = 0; from < node_count; from++) {
        int edge_count;
        is >> edge_count;
        for (int count = 0; count < edge_count; count++) {
            int to, weight;
            is >> to >> weight;
            g.edge_list[from].emplace_back(from, to, weight);
            g.reverse_edge_list[to].emplace_back(to, from, weight);
        }
    }
    return is;
}

class DfsInfo {
private:
    Graph const& graph;
    int current_depth;
    int current_end;
    std::vector<int> depth_nums;
    std::vector<int> end_nums;
    std::vector<WeightedEdge> tree_edges;
    std::vector<WeightedEdge> loop_edges;
    std::vector<WeightedEdge> forward_edges;
    std::vector<WeightedEdge> back_edges;
    std::vector<WeightedEdge> cross_edges;

    explicit DfsInfo(Graph const& graph) : graph(graph), current_depth(0), current_end(0),
                                           depth_nums(graph.nodeCount(), INT_MAX), end_nums(graph.nodeCount(), INT_MAX),
                                           tree_edges(), loop_edges(), forward_edges(), back_edges(), cross_edges() {
        // here we initialized everything we will want to make use of in DFS
    }

    void dfs(int start) {
        depth_nums[start] = ++current_depth;

        for (auto const& edge: graph.edges(start)) {
            if (depth_nums[edge.to] == INT_MAX) {
                tree_edges.push_back(edge);
                dfs(edge.to);
            } else if (depth_nums[edge.to] == current_depth) {
                loop_edges.push_back(edge);
            } else if (depth_nums[edge.to] > current_depth) {
                forward_edges.push_back(edge);
            } else if (end_nums[edge.to] == INT_MAX) {
                back_edges.push_back(edge);
            } else {
                cross_edges.push_back(edge);
            }
        }

        end_nums[start] = ++current_end;
    }

    void printEdges(std::vector<WeightedEdge> const& edges, std::ostream& os) const {
        for (WeightedEdge const& edge: edges)
            os << '\t' << edge.from << " -> " << edge.to << " (" << edge.weight << ") " << std::endl;
    }

public:
    void printResults(std::ostream& os = std::cout) const {
        os << "Total node count: " << graph.nodeCount() << std::endl;
        for (int node = 0; node < graph.nodeCount(); node++)
            os << "Node " << node << " started " << depth_nums[node] << ", ended " << end_nums[node] << std::endl;
        os << "One possible classification of the edges in the graph:" << std::endl;
        os << " - Tree edges:" << std::endl;
        printEdges(tree_edges, os);
        os << " - Loop edges:" << std::endl;
        printEdges(loop_edges, os);
        os << " - Forward edges:" << std::endl;
        printEdges(forward_edges, os);
        os << " - Back edges:" << std::endl;
        printEdges(back_edges, os);
        os << " - Cross edges:" << std::endl;
        printEdges(cross_edges, os);
    }

    std::vector<int> const& getDepthNums() const { return depth_nums; }

    std::vector<int> const& getEndNums() const { return end_nums; }

    std::vector<WeightedEdge> const& getTreeEdges() const { return tree_edges; }

    std::vector<WeightedEdge> const& getLoopEdges() const { return loop_edges; }

    std::vector<WeightedEdge> const& getForwardEdges() const { return forward_edges; }

    std::vector<WeightedEdge> const& getBackEdges() const { return back_edges; }

    std::vector<WeightedEdge> const& getCrossEdges() const { return cross_edges; }

    static DfsInfo dfs(Graph const& graph, int start) {
        DfsInfo di(graph);
        di.dfs(start);
        return di;
    }

    static DfsInfo dfs(Graph const& graph) {
        DfsInfo di(graph);

        for (int node = 0; node < graph.nodeCount(); node++)
            if (di.depth_nums[node] == INT_MAX)
                di.dfs(node);

        return di;
    }
};

std::vector<int> topological_sort(Graph const& graph) {
    DfsInfo dfs = DfsInfo::dfs(graph);

    if (!dfs.getLoopEdges().empty() || !dfs.getBackEdges().empty())
        throw std::runtime_error("Graph is not a DAG.");

    std::vector<int> ts(graph.nodeCount());
    for (int node = 0; node < graph.nodeCount(); node++)
        ts[graph.nodeCount() - dfs.getEndNums()[node]] = node;

    return ts;
}

std::vector<int> shortest_distance_from_s(Graph const& graph, int s) {
    std::vector<int> ts = topological_sort(graph);
    std::vector<int> distances(graph.nodeCount());

    int v_idx;
    // fill up nodes that are unreachable from s with infinity
    for (v_idx = 0; ts[v_idx] != s; v_idx++)
        distances[ts[v_idx]] = INT_MAX;
    // s has a distance of 0 from itself
    distances[ts[v_idx]] = 0;
    // for every other node, apply the DP method
    for (v_idx++; v_idx < graph.nodeCount(); v_idx++) {
        int v = ts[v_idx];

        int min_distance = INT_MAX;
        for (WeightedEdge const& rev_edge: graph.reverse_edges(v)) {
            if (distances[rev_edge.to] != INT_MAX) {
                // if the preceding node does not have a distance of infinity from s
                int distance = distances[rev_edge.to] + rev_edge.weight;
                if (distance < min_distance)
                    min_distance = distance;
            }
        }

        distances[v] = min_distance;
    }

    return distances;
}

class BfsInfo {
private:
    Graph const& graph;
    int current_visit;
    std::deque<int> next_to_visit;
    std::vector<int> visits;
    std::vector<int> distances;

    explicit BfsInfo(Graph const& graph) : graph(graph), current_visit(0), next_to_visit(),
                                           visits(graph.nodeCount(), INT_MAX), distances(graph.nodeCount(), INT_MAX) {
        // here we initialized everything we will want to make use of in BFS
    }

    void bfs(int s) {
        visits[s] = ++current_visit;
        distances[s] = 0;
        next_to_visit.push_back(s);
        do {
            int node = next_to_visit.front();
            next_to_visit.pop_front();

            for (Edge const& edge: graph.edges(node)) {
                if (visits[edge.to] == INT_MAX) {
                    visits[edge.to] = ++current_visit;
                    distances[edge.to] = distances[edge.from] + 1;
                    next_to_visit.emplace_back(edge.to);
                }
            }
        } while (!next_to_visit.empty());
    }

public:
    void printResults(std::ostream& os = std::cout) const {
        os << "Total node count: " << graph.nodeCount() << std::endl;
        for (int node = 0; node < graph.nodeCount(); node++) {
            os << "\t[ node " << node << " ]";
            if (visits[node] == INT_MAX)
                os << " was never reached";
            else
                os << " was #" << visits[node] << " with a distance of " << distances[node];
            os << std::endl;
        }
    }

    std::vector<int> const& getDistances() const { return distances; }

    static BfsInfo bfs(Graph const& graph, int start) {
        BfsInfo bfs_info(graph);
        bfs_info.bfs(start);
        return bfs_info;
    }
};

class DijkstraInfo {
private:
    Graph const& graph;
    std::vector<int> current_bests;
    std::vector<int> distances;

    explicit DijkstraInfo(Graph const& graph) : graph(graph), current_bests(graph.nodeCount(), INT_MAX),
                                                distances(graph.nodeCount(), INT_MAX) {
        // here we initialized everything we will want to make use of in BFS
    }

    int min() const {
        int min_node;

        for (min_node = 0; min_node < graph.nodeCount() &&
                           (current_bests[min_node] == INT_MIN || current_bests[min_node] == INT_MAX); min_node++);

        for (int node = min_node + 1; node < graph.nodeCount(); node++)
            if (current_bests[node] != INT_MIN && current_bests[node] != INT_MAX &&
                current_bests[node] < current_bests[min_node])
                min_node = node;

        return min_node;
    }

    void dijkstra(int start) {
        distances[start] = 0;
        for (WeightedEdge const& edge: graph.edges(start))
            current_bests[edge.to] = 0 + edge.weight;
        current_bests[start] = INT_MIN;

        int node;
        while ((node = min()) != graph.nodeCount()) {
            distances[node] = current_bests[node];
            current_bests[node] = INT_MIN;

            for (WeightedEdge const& edge: graph.edges(node)) {
                int distance = distances[node] + edge.weight;
                if (distance < current_bests[edge.to])
                    current_bests[edge.to] = distance;
            }
        }
    }

public:
    std::vector<int> const& getDistances() const { return distances; }

    static DijkstraInfo dijkstra(Graph const& graph, int start) {
        DijkstraInfo dijkstraInfo(graph);
        dijkstraInfo.dijkstra(start);
        return dijkstraInfo;
    }
};

class PrimInfo {
private:
    Graph const& graph;
    std::vector<bool> covered;
    std::vector<WeightedEdge> tree_edges;
    MinHeap<WeightedEdge> next_edges;

    explicit PrimInfo(Graph const& graph) : graph(graph), covered(graph.nodeCount()) {}

public:
    std::vector<WeightedEdge> const& getTreeEdges() const { return tree_edges; }

    void prim(int s) {
        do {
            for (WeightedEdge const& edge: graph.edges(s))
                next_edges.insert(edge);

            WeightedEdge edge_to_take;
            while (!next_edges.empty() && covered[(edge_to_take = next_edges.extreme()).to])
                next_edges.extreme_remove();

            if (next_edges.empty())
                throw std::runtime_error("Graph isn't connected");

            tree_edges.push_back(edge_to_take);
            s = edge_to_take.to;
            covered[s] = true;
        } while (tree_edges.size() != graph.nodeCount() - 1);
    }

    static PrimInfo prim(Graph const& graph, int start) {
        PrimInfo primInfo(graph);
        primInfo.prim(start);
        return primInfo;
    }
};

int main() {
    Graph g;
    int s;
    std::cin >> g >> s;

//    std::vector<std::pair<std::string, std::vector<int>>> test_cases;
//
//    DijkstraInfo dijkstra = DijkstraInfo::dijkstra(g, s);
//    std::vector<int> dijkstra_distances = dijkstra.getDistances();
//    test_cases.emplace_back("Dijkstra", dijkstra_distances);
//
//    std::vector<int> dag_distances = shortest_distance_from_s(g, s);
//    test_cases.emplace_back("DFS (on DAG)", dag_distances);
//
//    BfsInfo bfs = BfsInfo::bfs(g, s);
//    test_cases.emplace_back("BFS", dag_distances);
//
//    for (auto const& test_case: test_cases) {
//        std::cout << "<< " << test_case.first << " >>" << std::endl;
//        std::vector<int> const& distances = test_case.second;
//
//        std::cout << "The shortest distance to each node from " << s << " is:" << std::endl;
//        for (int node = 0; node < g.nodeCount(); node++) {
//            std::cout << "\t [ node " << node << " ] = ";
//            if (distances[node] != INT_MAX)
//                std::cout << distances[node];
//            else
//                std::cout << "infinity";
//            std::cout << std::endl;
//        }
//
//        std::cout << std::endl;
//    }

//    std::cout << "Prim's algortihm went like:" << std::endl;
//
//    PrimInfo prim = PrimInfo::prim(g, s);
//    std::vector<WeightedEdge> tree_edges = prim.getTreeEdges();
//    for (auto const& edge : tree_edges)
//        std::cout << edge.from << " -> " << edge.to << " (" << edge.weight << ")" << std::endl;

    return 0;
}
