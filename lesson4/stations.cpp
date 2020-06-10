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

vector<string> load_stations(string path_to_stations_file) {
  std::string str;
  std::ifstream ifs(path_to_stations_file);
  std::smatch results;
  vector<string> stations;

  if (ifs.fail()) {
    throw "Failed to open station file.";
  }
  while (getline(ifs, str)) {
    if (std::regex_match(str, results, std::regex("(\\d+)\\t(\\D+)"))) {
      stations.push_back(results[2].str());
    }
  }
  return stations;
}

vector<vector<pair<int, int>>> load_links(string path_to_links_file,
                                          int num_of_nodes) {
  std::string str;
  std::ifstream ifs(path_to_links_file);
  std::smatch results;
  int node_id, adjacent_node_id, weight;
  vector<pair<int, int>> tmp(0, make_pair(0, 0));
  vector<vector<pair<int, int>>> graph(num_of_nodes, tmp);

  if (ifs.fail()) {
    throw "Failed to open station file.";
  }
  while (getline(ifs, str)) {
    // debug(str);
    if (std::regex_match(str, results,
                         std::regex("(\\d+)\\t(\\d+)\\t(\\d+)"))) {
      node_id = stoi(results[1].str());
      adjacent_node_id = stoi(results[2].str());
      weight = stoi(results[3].str());
      graph[node_id].push_back(make_pair(weight, adjacent_node_id));
      graph[adjacent_node_id].push_back(make_pair(weight, node_id));
    }
  }
  return graph;
}

int search_station(const vector<string>& stations, string target_station) {
  auto iterator_of_target =
      find(stations.begin(), stations.end(), target_station);
  if (iterator_of_target == stations.end()) {
    throw "Invalid username";
  }
  int node_id_of_target = iterator_of_target - stations.begin();
  return node_id_of_target;
}

const int VERY_LARGE_NUM = 1e6;

void dijkstra(int start_node_id, int target_node_id,
              const vector<vector<pair<int, int>>>& graph,
              vector<int>& visit_status, vector<int>& distance,
              vector<int>& parent) {
  distance[start_node_id] = 0;
  visit_status[start_node_id] = 1;
  priority_queue<pair<int, int>, vector<pair<int, int>>,
                 greater<pair<int, int>>>
      PQ;
  PQ.push(make_pair(0, start_node_id));
  pair<int, int> node;
  int node_id, next_node_id, next_node_weight;
  while (!PQ.empty()) {
    node = PQ.top();
    PQ.pop();
    node_id = node.second;
    visit_status[node_id] = 2;
    if (distance[node_id] < node.first) continue;
    for (int i = 0; i < graph[node_id].size(); i++) {
      next_node_id = graph[node_id][i].second;
      next_node_weight = graph[node_id][i].first;
      if (visit_status[next_node_id] != 2) {
        if (distance[next_node_id] > distance[node_id] + next_node_weight) {
          distance[next_node_id] = distance[node_id] + next_node_weight;
          visit_status[next_node_id] = 1;
          parent[next_node_id] = node_id;
          PQ.push(make_pair(distance[next_node_id], next_node_id));
          if (next_node_id == target_node_id) {
            return;
          }
        }
      }
    }
  }
}

int main() {
  vector<string> stations = load_stations("./data/stations/stations.txt");
  int num_of_nodes = stations.size();
  vector<vector<pair<int, int>>> graph =
      load_links("./data/stations/edges.txt", num_of_nodes);

  string start_station, target_station;
  cin >> start_station >> target_station;

  int start_node_id = search_station(stations, start_station);
  int target_node_id = search_station(stations, target_station);

  vector<int> visit_status(num_of_nodes), distance(num_of_nodes, 1e5),
      parent(num_of_nodes, -1);
  dijkstra(start_node_id, target_node_id, graph, visit_status, distance,
           parent);
  vector<int> path_store;
  if (distance[target_node_id] == 1e5) {
    cout << "Unreachable!" << endl;
  } else {
    cout << "Reachable!" << endl;
    cout << "Needed time [min]: " << distance[target_node_id] << endl;
    int parent_node_id = parent[target_node_id];
    while (parent_node_id != start_node_id) {
      path_store.push_back(parent_node_id);
      parent_node_id = parent[parent_node_id];
    }
    cout << stations[start_node_id] << " -> ";
    for (int i = path_store.size() - 1; i >= 0; i--) {
      cout << stations[path_store[i]] << " -> ";
    }
    cout << stations[target_node_id] << endl;
  }
}