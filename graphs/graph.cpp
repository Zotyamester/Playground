#include <iostream>
#include <vector>
#include <exception>
#include <climits>

struct Edge {
    int from, to;

    Edge(int from, int to) : from(from), to(to) {}
};

struct WeightedEdge : public Edge {
    int weight;

    WeightedEdge(int from, int to, int weight) : Edge(from, to), weight(weight) {}
};

class Graph {
private:
    int n;
    std::vector<std::vector<WeightedEdge>> edge_list;
    std::vector<std::vector<WeightedEdge>> reverse_edge_list;
public:
    Graph() : edge_list(), reverse_edge_list() {}
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
            g.edge_list[from].push_back(WeightedEdge(from, to, weight));
            g.reverse_edge_list[to].push_back(WeightedEdge(to, from, weight));
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

    DfsInfo(Graph const& graph) :  graph(graph),
                                   current_depth(0), current_end(0),
                                   depth_nums(graph.nodeCount(), INT_MAX),
                                   end_nums(graph.nodeCount(),   INT_MAX),
                                   tree_edges(), loop_edges(),
                                   forward_edges(), back_edges(),
                                   cross_edges()
    {
        // here we initialized everything we will want to make use of in DFS
    }

    void printEdges(std::vector<WeightedEdge> const& edges, std::ostream& os) const {
        for (auto edge : edges)
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

    void dfs(int start) {
        depth_nums[start] = ++current_depth;
        
        for (auto const& edge : graph.edges(start)) {
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

    static DfsInfo dfs(Graph const& graph, int start) {
        DfsInfo di(graph);

        for (int node = 0; node < graph.nodeCount(); node++)
            if (di.depth_nums[node] == INT_MAX)
                di.dfs(node);

        return di;
    }
};

std::vector<int> topological_sort(Graph const& graph) {
    DfsInfo dfs = DfsInfo::dfs(graph, 0);

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
    // for every other node, apply the DP mehthod
    for (v_idx++; v_idx < graph.nodeCount(); v_idx++) {
        int v = ts[v_idx];

        int min_distance = INT_MAX;
        for (auto const& rev_edge : graph.reverse_edges(v)) {
            if (distances[rev_edge.to] != INT_MAX) {
                // if the preceeding node does not have a distance of infinity from s
                int distance = distances[rev_edge.to] + rev_edge.weight;
                if (distance < min_distance)
                    min_distance = distance;
            }
        }

        distances[v] = min_distance;
    }

    return distances;
}

int main() {
    Graph g;
    int s;
    std::cin >> g >> s;

    std::vector<int> distances = shortest_distance_from_s(g, s);
    std::cout << "The shortest distance to each node from " << s << " is:" << std::endl;
    for (int node = 0; node < g.nodeCount(); node++) {
        std::cout << "\t [ node " << node << " ] = ";
        if (distances[node] != INT_MAX)
            std::cout << distances[node];
        else
            std::cout << "infinity";
        std::cout << std::endl;
    }
    return 0;
}
