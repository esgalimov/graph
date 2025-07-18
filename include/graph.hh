#ifndef MY_GRAPH
#define MY_GRAPH

#include <vector>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <string>

#include <initializer_list>
#include <utility>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <ranges>
#include <filesystem>


namespace graph {
    constexpr int EMPTY = -1;
    constexpr int ALIGNMENT = 6;

    using Stack = typename std::stack<int>;
    using Queue = typename std::queue<int>;
    using VecInt = typename std::vector<int>;
    using VecStr = typename std::queue<std::string>;

    template <typename T>
        concept AllowedContainer = std::same_as<T, Stack> || std::same_as<T, Queue>;

    // TODO: graphviz dumps
    //       gtest
    //       list-init
    //       graph traversals
    //       think about additional info of graph nodes

    class graph_buffer_t {
    protected:
        VecInt edges_;
        VecInt next_; // node's edges list
        VecInt prev_;

        int size_ = 0;
        
        void init(int size) {
            size_ = size;

            edges_.reserve(size_);
            next_.resize(size_, EMPTY);
            prev_.resize(size_, EMPTY);
        }
    };

    class graph_t : private graph_buffer_t {
        using graph_buffer_t::edges_;
        using graph_buffer_t::next_;
        using graph_buffer_t::prev_;

        using graph_buffer_t::size_;

        using edge_t = std::pair<int, int>;

        std::set<edge_t> edges_set;
        std::map<int, std::set<int>> nodes_pos_map;
        std::map<int, int>           nodes_names_id;
        int empty_part_size = 0;

        int edges_cnt = 0;
        int nodes_cnt = 0;


        void fill_names_id() {
            for (auto&& pair: nodes_names_id) edges_[pair.second] = pair.first;
        }

        void fill_edges() {
            edges_.resize(empty_part_size, EMPTY);

            for (auto&& ed: edges_set) {
                edges_.push_back(ed.first); 
                nodes_pos_map[ed.first].emplace(edges_.size() - 1);

                edges_.push_back(ed.second);
                nodes_pos_map[ed.second].emplace(edges_.size() - 1);

                nodes_names_id.try_emplace(ed.first, nodes_names_id.size());
                nodes_names_id.try_emplace(ed.second, nodes_names_id.size());
            }
        }

        void fill_node_edge_list() {
            int start_pos = 0;

            for (auto&& pair: nodes_pos_map) {
                int saved_start_pos = start_pos;
                int pos = start_pos;

                for (auto&& elem: pair.second) {
                    int next_pos = elem ^ 1;
                    next_[pos] = next_pos;
                    pos = next_pos;
                }
                next_[pos] = saved_start_pos;

                pos = start_pos;

                for (auto&& elem: pair.second | std::views::reverse) {
                    int prev_pos = elem ^ 1;
                    prev_[pos] = prev_pos;
                    pos = prev_pos;
                }
                prev_[pos] = saved_start_pos;
                start_pos++;
            }
        }

        void fill_table() {
            fill_edges();
            fill_names_id();
            fill_node_edge_list();
        }

        template <AllowedContainer T> VecInt fs(int start_node_name) {
            T search_stk;

            std::vector<bool> used(nodes_cnt, false);
            VecInt  result_fs;

            used[nodes_names_id[start_node_name]] = true;

            search_stk.push(start_node_name);

            while (!search_stk.empty()) {
                int top = 0;

                if constexpr (std::same_as<T, Stack>) top = search_stk.top();
                else                                  top = search_stk.front();

                search_stk.pop();

                result_fs.push_back(top);

                VecInt neighbors = get_neighbors(top);

                for (auto&& x: neighbors) {
                    if (!used[nodes_names_id[x]]) {
                        search_stk.push(x);
                        used[nodes_names_id[x]] = true;
                    }
                }
            }
            return result_fs;
        }

        public:

        graph_t(std::initializer_list<edge_t> init_list) : edges_set(init_list) {
            for_each(init_list.begin(), init_list.end(), 
                    [this](const edge_t ed) { nodes_pos_map.emplace(ed.first,  std::set<int>()); 
                                              nodes_pos_map.emplace(ed.second, std::set<int>()); });
            
            empty_part_size = (nodes_pos_map.size() % 2) ? nodes_pos_map.size() + 1 : nodes_pos_map.size();

            init(edges_set.size() * 2 + empty_part_size);

            fill_table();

            edges_cnt = edges_set.size();
            nodes_cnt = nodes_pos_map.size();
        } 

        VecInt dfs(int start_node_name) { return fs<Stack>(start_node_name); }

        VecInt bfs(int start_node_name) { return fs<Queue>(start_node_name); }

        VecInt get_neighbors(int start_node_name) {
            VecInt ret;

            int start_list_index = nodes_names_id[start_node_name];
            int i = next_[start_list_index];

            while (i != start_list_index) { 
                ret.push_back(edges_[i]);
                i = next_[i];
            } 

            return ret;
        }
// ---------------------- text dumps ----------------------
        void dump() { 
            for (int i = 0; i < size_; ++i) std::cout << std::setw(ALIGNMENT) << i;
            std::cout << std::endl;

            for (auto&& x : edges_)         std::cout << std::setw(ALIGNMENT) << x;
            std::cout << std::endl; 

            for (auto&& x : next_)         std::cout << std::setw(ALIGNMENT) << x;
            std::cout << std::endl; 

            for (auto&& x : prev_)         std::cout << std::setw(ALIGNMENT) << x;
            std::cout << std::endl << std::endl; 
            
            std::cout << "POSITION MAP" << std::endl;
            for (auto&& pair: nodes_pos_map) {
                std::cout << pair.first << ": ";
                for (auto&& elem: pair.second) std::cout << elem << " ";
                std::cout << std::endl;
            }
        }
    
    private:

    int open_grapviz(std::ofstream& out, std::string& dot_pth) const {
        out.open(dot_pth);
        if (!out.is_open()) {
            std::cerr << "Cannot open graphviz file\n";
            return 1;
        }

        out << "graph\n{\n  rankdir=LR;\n";
        return 0;
    }

    void close_graphviz(std::ofstream& out) const {
        out << "}";
        out.close();
    }

    public:
// ---------------------- graphviz dumps ----------------------

    void graphviz_dump() {
        namespace fs = std::filesystem;

        auto file_folder    = fs::absolute(__FILE__).parent_path().parent_path();
        fs::path pth        = fs::canonical(file_folder.string());
        std::string dot_pth = pth.string() + "/dumps/dump.dot";

        std::ofstream out;
        open_grapviz(out, dot_pth);

        for (int i = 0; i < empty_part_size; ++i) {
            out << "    node_" << edges_[i] << "[shape = Mrecord, label = \"" << edges_[i] << "\"];\n";
        }

        for (int i = empty_part_size; i < size_; i = 1 + (i ^ 1)) {
            out << "    node_" << edges_[i] << "--" << "node_" << edges_[i ^ 1] << ";\n";
        }

        close_graphviz(out);

        std::string cmd = "dot " + dot_pth + " -Tpng -o " + pth.string() + "/dumps/dump.png";
        std::system(cmd.data());
    }

    };
} // <-- namespace graph

#endif
