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

vector<string> load_nicknames(string path_to_nicknames_file) {
  std::string str;
  std::ifstream ifs(path_to_nicknames_file);
  std::smatch results;
  vector<string> nicknames;

  if (ifs.fail()) {
    throw "Failed to open nickname file.";
  }
  while (getline(ifs, str)) {
    if (std::regex_match(str, results, std::regex("(\\d+)\t(\\w+)"))) {
      nicknames.push_back(results[2].str());
    }
  }
  return nicknames;
}

vector<vector<int>> load_links(string path_to_links_file, int num_of_nodes) {
  std::string str;
  std::ifstream ifs(path_to_links_file);
  std::smatch results;
  int node_id, adjacent_node_id;
  vector<vector<int>> graph(num_of_nodes, vector<int>{});

  if (ifs.fail()) {
    throw "Failed to open nickname file.";
  }
  while (getline(ifs, str)) {
    if (std::regex_match(str, results, std::regex("(\\d+)\t(\\d+)"))) {
      node_id = stoi(results[1].str());
      adjacent_node_id = stoi(results[2].str());
      graph[node_id].push_back(adjacent_node_id);
    }
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

int search_nickname(const vector<string>& nicknames, string target_nickname) {
  auto iterator_of_target =
      find(nicknames.begin(), nicknames.end(), target_nickname);
  if (iterator_of_target == nicknames.end()) {
    throw "Invalid username";
  }
  int node_id_of_target = iterator_of_target - nicknames.begin();
  return node_id_of_target;
}

int main() {
  vector<string> nicknames = load_nicknames("./data/dummy_sns/nicknames.txt");
  int num_of_nodes = nicknames.size();
  vector<vector<int>> graph =
      load_links("./data/dummy_sns/links.txt", num_of_nodes);

  string start_nickname, target_nickname;
  cin >> start_nickname >> target_nickname;

  int start_node_id = search_nickname(nicknames, start_nickname);
  int target_node_id = search_nickname(nicknames, target_nickname);

  vector<int> visit_status(num_of_nodes), distance(num_of_nodes, -1),
      parent(num_of_nodes, -1);
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
    cout << nicknames[start_node_id] << " -> ";
    for (int i = path_store.size() - 1; i >= 0; i--) {
      cout << nicknames[path_store[i]] << " -> ";
    }
    cout << nicknames[target_node_id] << endl;
  }
  return 0;
}