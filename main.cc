#include <iostream> 
#include "graph.hh"

int main() try {
    graph::graph_t<std::string> gr{{"a", "b"}, {"b", "d"}, {"c", "d"}, {"a", "c"}, {"b", "c"}, {"d", "hello"}};

    auto fs = gr.dfs("1");
    std::cout << "DFS: ";
    for (auto&& x: fs) std::cout << x << " ";
    std::cout << std::endl;

    fs = gr.bfs("1");
    std::cout << "BFS: ";
    for (auto&& x: fs) std::cout << x << " ";
    std::cout << std::endl << std::endl;

    gr.dump();
    gr.graphviz_dump();

    return 0;
}
catch (std::exception& exc) {
    std::cout << exc.what() << std::endl;
}