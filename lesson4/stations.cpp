#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

#include "csv.h"

class Graph {
 public:
  static constexpr int VERY_LARGE_NUM = 1e9;
  Graph(int numOfNodes)
      : numOfNodes_(numOfNodes),
        graph_(numOfNodes, std::vector<std::pair<int, int>>{}),
        visitStatus_(numOfNodes, 0),
        distance_(numOfNodes, VERY_LARGE_NUM),
        parent_(numOfNodes, -1) {}

  // Load links from tsv file and convert it to graph object.
  void loadGraph(std::string path_to_links_file) {
    io::CSVReader<3, io::trim_chars<' '>, io::no_quote_escape<'\t'>>
        graphReader(path_to_links_file);
    int currentNodeId, adjacentNodeId, weight;
    while (graphReader.read_row(currentNodeId, adjacentNodeId, weight)) {
      // graph_ is a vector of vector of pair, whose element is (weight,
      // adjacent node id).
      graph_[currentNodeId].push_back(std::make_pair(weight, adjacentNodeId));
      graph_[adjacentNodeId].push_back(std::make_pair(weight, currentNodeId));
    }
  }

  // Set startNodeId_ and targetNodeId_.
  void setStartTargetNode(int startNodeId, int targetNodeId) {
    startNodeId_ = startNodeId;
    targetNodeId_ = targetNodeId;
  }

  // solve dijkstra to find the shortest path from the start node and the target
  // node.
  void dijkstra() {
    // Initialize visitStatus_, distance_, parent_.
    // visitStatus_ is 0 zero if the node is not visited, 2 when the visiting
    // has completed, otherwise 1. Initizlized to 0.
    resetVectors();
    distance_[startNodeId_] = 0;
    visitStatus_[startNodeId_] = 1;
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                        std::greater<std::pair<int, int>>>
        PQ;
    PQ.push(std::make_pair(0, startNodeId_));
    std::pair<int, int> node;
    int nodeId, nextNodeId, nextNodeWeight;
    while (!PQ.empty()) {
      node = PQ.top();
      PQ.pop();
      nodeId = node.second;
      visitStatus_[nodeId] = 2;
      if (distance_[nodeId] < node.first) continue;
      for (int i = 0; i < graph_[nodeId].size(); i++) {
        nextNodeId = graph_[nodeId][i].second;
        nextNodeWeight = graph_[nodeId][i].first;
        if (visitStatus_[nextNodeId] != 2) {
          if (distance_[nextNodeId] > distance_[nodeId] + nextNodeWeight) {
            distance_[nextNodeId] = distance_[nodeId] + nextNodeWeight;
            visitStatus_[nextNodeId] = 1;
            parent_[nextNodeId] = nodeId;
            PQ.push(std::make_pair(distance_[nextNodeId], nextNodeId));
            // Stop searching if reached the targetNode
            if (nextNodeId == targetNodeId_) {
              return;
            }
          }
        }
      }
    }
  }

  // Check if the targetNodeId_ is reachable from the StartNodeId_ after
  // dijkstra
  bool const ifReachable() {
    if (distance_[targetNodeId_] == VERY_LARGE_NUM) {
      std::cout << "Unreachable!" << std::endl;
      return 0;
    } else {
      std::cout << "Reachable!" << std::endl;
      return 1;
    }
  }

  // Get the shortest path from StartNodeId_ to targetNodeId_ after dijkstra
  std::vector<int> const getShortestPath() {
    std::vector<int> path;
    std::cout << "Takes " << distance_[targetNodeId_] << " minutes"
              << std::endl;
    int parentNodeId = parent_[targetNodeId_];
    while (parentNodeId != startNodeId_) {
      path.push_back(parentNodeId);
      parentNodeId = parent_[parentNodeId];
    }
    return path;
  }

 private:
  std::vector<std::vector<std::pair<int, int>>> graph_;
  int numOfNodes_, startNodeId_, targetNodeId_;
  std::vector<int> visitStatus_, distance_, parent_;

  void resetVectors() {
    std::fill(visitStatus_.begin(), visitStatus_.end(), 0);
    std::fill(distance_.begin(), distance_.end(), VERY_LARGE_NUM);
    std::fill(parent_.begin(), parent_.end(), -1);
  }
};

std::vector<std::string> loadStations(std::string pathToStationsFile) {
  io::CSVReader<2, io::trim_chars<' '>, io::no_quote_escape<'\t'>>
      StationsReader(pathToStationsFile);
  int nodeId;
  std::string stationName;
  std::vector<std::string> stations;
  while (StationsReader.read_row(nodeId, stationName)) {
    stations.push_back(stationName);
  }
  return stations;
}

// Search stations and return the node id for the input station.
int getNodeIdFromStationName(const std::vector<std::string>& stations,
                             std::string stationName) {
  auto iteratorOfTarget = find(stations.begin(), stations.end(), stationName);
  // If the input was not found, throw exception.
  if (iteratorOfTarget == stations.end()) {
    throw std::invalid_argument("No station named '" + stationName + "'");
  }
  int nodeIdOfTarget = iteratorOfTarget - stations.begin();
  return nodeIdOfTarget;
}

// Print the formatted path.
void printPath(int startNodeId, int targetNodeId, std::vector<int>& path,
               const std::vector<std::string>& stations) {
  std::cout << stations[startNodeId] << " -> ";
  for (int i = path.size() - 1; i >= 0; i--) {
    std::cout << stations[path[i]] << " -> ";
  }
  std::cout << stations[targetNodeId] << std::endl;
}

void check(Graph& graphObj, const std::vector<std::string>& stations,
           std::string startStationName, std::string targetStationName) {
  int startNodeId, targetNodeId;
  try {
    startNodeId = getNodeIdFromStationName(stations, startStationName);
    targetNodeId = getNodeIdFromStationName(stations, targetStationName);
  } catch (std::invalid_argument e) {  // If there is no matching station
    std::cerr << e.what() << std::endl;
    return;
  }
  // Set startNodeId and targetNodeId to the graph object
  graphObj.setStartTargetNode(startNodeId, targetNodeId);
  // Couduct breadth first search to find the shortest path
  graphObj.dijkstra();
  // If reachable, get the shortest path and print
  if (graphObj.ifReachable()) {
    std::vector<int> path = graphObj.getShortestPath();
    printPath(startNodeId, targetNodeId, path, stations);
  }
}

void runTest(Graph& graphObj, const std::vector<std::string>& stations) {
  check(graphObj, stations, "東京", "大手町");  // can reach directory
  check(graphObj, stations, "東京", "幕張");    // can reach via some stations
  check(graphObj, stations, "hoge", "fuga");    // no such station
  check(graphObj, stations, "", "");            // empty input
}

int main() {
  // Load station names from tsv file
  std::vector<std::string> stations =
      loadStations("./data/stations/stations.txt");
  int numOfNodes = stations.size();
  // Initialize graph object
  Graph graphObj(numOfNodes);
  graphObj.loadGraph("./data/stations/edges.txt");
  std::cout << "loading finished" << std::endl;
  runTest(graphObj, stations);

  std::string startStationName, targetStationName;
  while (1) {
    std::cout << "Please input start and target station name" << std::endl;
    std::cin >> startStationName >> targetStationName;
    check(graphObj, stations, startStationName, targetStationName);
  }
  return 0;
}