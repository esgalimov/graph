#include <iostream> 
#include "graph.hh"

int main() {
    graph::graph_t gr{{1, 2}, {2, 4}, {3, 4}, {1, 3}, {2, 3}};

    auto fs = gr.dfs(1);
    std::cout << "DFS: ";
    for (auto&& x: fs) std::cout << x << " ";
    std::cout << std::endl;

    fs = gr.bfs(1);
    std::cout << "BFS: ";
    for (auto&& x: fs) std::cout << x << " ";
    std::cout << std::endl << std::endl;

    gr.dump();
    gr.graphviz_dump();

    return 0;
}