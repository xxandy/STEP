#include <algorithm>
#include <bitset>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <regex>
#include <set>
#include <string>
#include <vector>

#include "csv.h"

// Acknowledgement: Special thanks to kyomukyomupurin, who developed this
// template.
template <class T, class U>
std::ostream& operator<<(std::ostream& os, const std::pair<T, U>& p) {
  return os << '(' << p.first << ", " << p.second << ')';
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
  int n = 0;
  for (auto e : vec) os << (n++ ? ", " : "{") << e;
  return os << (n ? "}" : "{}");
}

template <class T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& st) {
  int n = 0;
  for (auto e : st) os << (n++ ? ", " : "{") << e;
  return os << (n ? "}" : "{}");
}

template <class T, class U>
std::ostream& operator<<(std::ostream& os, const std::map<T, U>& mp) {
  int n = 0;
  for (auto e : mp) os << (n++ ? ", " : "{") << e;
  return os << (n ? "}" : "{}");
}

template <class T>
std::istream& operator>>(std::istream& is, std::vector<T>& vec) {
  for (T& e : vec) is >> e;
  return is;
}

#ifdef LOCAL
#define debug(...) \
  std::cerr << "[" << #__VA_ARGS__ << "]: ", debug_out(__VA_ARGS__)
#else
#define debug(...)
#endif

void debug_out() { std::cerr << '\n'; }

template <class Head, class... Tail>
void debug_out(Head&& head, Tail&&... tail) {
  std::cerr << head;
  if (sizeof...(Tail) != 0) std::cerr << ", ";
  debug_out(std::forward<Tail>(tail)...);
}
using namespace std;
using int64 = long long;

vector<string> load_pages(string path_to_pages_file) {
  io::CSVReader<2, io::trim_chars<' '>, io::no_quote_escape<'\t'>> pages_reader(
      path_to_pages_file);
  int node_id;
  string page_title;
  vector<string> pages;
  while (pages_reader.read_row(node_id, page_title)) {
    pages.push_back(page_title);
  }
  return pages;
}

vector<vector<int>> load_links(string path_to_links_file, int num_of_nodes) {
  vector<vector<int>> graph(num_of_nodes, vector<int>{});
  io::CSVReader<2, io::trim_chars<' '>, io::no_quote_escape<'\t'>> graph_reader(
      path_to_links_file);
  int current_node_id, adjacent_node_id;
  while (graph_reader.read_row(current_node_id, adjacent_node_id)) {
    graph[current_node_id].push_back(adjacent_node_id);
  }
  return graph;
}

void breadth_first_search(int start_node_id, int target_node_id,
                          const vector<vector<int>>& graph,
                          vector<int>& visit_status, vector<int>& distance,
                          vector<int>& parent) {
  int node_id, next_node_id;
  queue<int> Q;
  distance[start_node_id] = 0;
  visit_status[start_node_id] = 1;
  Q.push(start_node_id);
  while (!Q.empty()) {
    node_id = Q.front();
    Q.pop();
    for (int i = 0; i < graph[node_id].size(); i++) {
      next_node_id = graph[node_id][i];
      if (visit_status[next_node_id] == 0) {
        Q.push(next_node_id);
        visit_status[next_node_id] = 1;
        distance[next_node_id] = distance[node_id] + 1;
        parent[next_node_id] = node_id;
        if (next_node_id == target_node_id) {
          return;
        }
      }
    }
    visit_status[node_id] = 2;
  }
}

int search_page_title(const vector<string>& pages, string target_page_title) {
  auto iterator_of_target = find(pages.begin(), pages.end(), target_page_title);
  if (iterator_of_target == pages.end()) {
    throw "Invalid username";
  }
  int node_id_of_target = iterator_of_target - pages.begin();
  return node_id_of_target;
}

int main() {
  vector<string> pages = load_pages("./data/wikipedia/pages.txt");
  int num_of_nodes = pages.size();
  vector<vector<int>> graph =
      load_links("./data/wikipedia/links.txt", num_of_nodes);

  string start_page_title, target_page_title;
  cin >> start_page_title >> target_page_title;

  int start_node_id = search_page_title(pages, start_page_title);
  int target_node_id = search_page_title(pages, target_page_title);

  const int VERY_LARGE_NUM = 1e9;
  vector<int> visit_status(num_of_nodes),
      distance(num_of_nodes, VERY_LARGE_NUM), parent(num_of_nodes, -1);
  breadth_first_search(start_node_id, target_node_id, graph, visit_status,
                       distance, parent);
  vector<int> path_store;

  if (distance[target_node_id] == -1) {
    cout << "Unreachable!" << endl;
  } else {
    cout << "Reachable!" << endl;
    cout << "Length of the shortest path: " << distance[target_node_id] << endl;
    int parent_node_id = parent[target_node_id];
    while (parent_node_id != start_node_id) {
      path_store.push_back(parent_node_id);
      parent_node_id = parent[parent_node_id];
    }
    cout << pages[start_node_id] << " -> ";
    for (int i = path_store.size() - 1; i >= 0; i--) {
      cout << pages[path_store[i]] << " -> ";
    }
    cout << pages[target_node_id] << endl;
  }
  return 0;
}