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
        graph_(numOfNodes, std::vector<int>{}),
        visitStatus_(numOfNodes, 0),
        distance_(numOfNodes, VERY_LARGE_NUM),
        parent_(numOfNodes, -1) {}

  // Load links from tsv file and convert it to graph object.
  void loadGraph(std::string path_to_links_file) {
    io::CSVReader<2, io::trim_chars<' '>, io::no_quote_escape<'\t'>>
        graphReader(path_to_links_file);
    int currentNodeId, adjacentNodeId;
    while (graphReader.read_row(currentNodeId, adjacentNodeId)) {
      // graph_ is a vector of vector, whose element is a adjacent node id.
      graph_[currentNodeId].push_back(adjacentNodeId);
       // ALEXNOTE:  rather than maintaing the nicknames and the graph separately,
        //             you would get more performance by having Node objects store links
        //              (in C++, pointers would be ideal) directly to their adjacent notes.
    }
  }

  // Set startNodeId_ and targetNodeId_.
  void setStartTargetNode(int startNodeId, int targetNodeId) {
    startNodeId_ = startNodeId;
    targetNodeId_ = targetNodeId;
  }

  // Conduct breadth first search (BFS) to find the shortest path from
  // startNodeId_ to targetNodeId_.
  void breadthFirstSearch() {
    // Initialize visitStatus_, distance_, parent_.
    // visitStatus_ is 0 zero if the node is not visited, 2 when the visiting
    // has completed, otherwise 1. Initizlized to 0.
    resetVectors();
    int nodeId, nextNodeId;
    std::queue<int> Q;
    distance_[startNodeId_] = 0;
    visitStatus_[startNodeId_] = 1;
    Q.push(startNodeId_);
    while (!Q.empty()) {
      nodeId = Q.front();
      Q.pop();
      for (int i = 0; i < graph_[nodeId].size(); i++) {
        nextNodeId = graph_[nodeId][i];
        if (visitStatus_[nextNodeId] == 0) {
          Q.push(nextNodeId);
          visitStatus_[nextNodeId] = 1;
          distance_[nextNodeId] = distance_[nodeId] + 1;
          parent_[nextNodeId] = nodeId;
          // Stop searching if reached the targetNode
          if (nextNodeId == targetNodeId_) {
            return;
          }
        }
      }
      visitStatus_[nodeId] = 2;
    }
  }

  // Check if the targetNodeId_ is reachable from the StartNodeId_ after BFS
  bool const ifReachable() {
    if (distance_[targetNodeId_] == VERY_LARGE_NUM) {
      std::cout << "Unreachable!" << std::endl;
      return 0;
    } else {
      std::cout << "Reachable!" << std::endl;
      return 1;
    }
  }

  // Get the shortest path from StartNodeId_ to targetNodeId_ after BFS
  std::vector<int> const getShortestPath() {
    std::vector<int> path;
    std::cout << "Length of the shortest path: " << distance_[targetNodeId_]
              << std::endl;
    int parentNodeId = parent_[targetNodeId_];
    while (parentNodeId != startNodeId_) {
      path.push_back(parentNodeId);
      parentNodeId = parent_[parentNodeId];
    }
    return path;
  }

 private:
  std::vector<std::vector<int>> graph_;
  int numOfNodes_, startNodeId_, targetNodeId_;
  std::vector<int> visitStatus_, distance_, parent_;

  void resetVectors() {
    std::fill(visitStatus_.begin(), visitStatus_.end(), 0);
    std::fill(distance_.begin(), distance_.end(), VERY_LARGE_NUM);
    std::fill(parent_.begin(), parent_.end(), -1);
  }
};

// Load list of nicknames from tsv file.
std::vector<std::string> loadnicknames(std::string path_to_nicknames_file) {
  io::CSVReader<2, io::trim_chars<' '>, io::no_quote_escape<'\t'>>
      nicknames_reader(path_to_nicknames_file);
  int nodeId;
  std::string nickname;
  std::vector<std::string> nicknames;
  while (nicknames_reader.read_row(nodeId, nickname)) {
    nicknames.push_back(nickname);
  }
  return nicknames;
}

// Search nicknames and return the node id for the input nickname.
int getNodeIdFromNickname(const std::vector<std::string>& nicknames,
                          std::string nickname) {
  auto iteratorOfTarget = find(nicknames.begin(), nicknames.end(), nickname);
  // If the input was not found, throw exception.
  if (iteratorOfTarget == nicknames.end()) {
    throw std::invalid_argument("No nickname named '" + nickname + "'");
  }
  int nodeIdOfTarget = iteratorOfTarget - nicknames.begin();
  return nodeIdOfTarget;
}

// Print the formatted path.
void printPath(int startNodeId, int targetNodeId, std::vector<int>& path,
               const std::vector<std::string>& nicknames) {
  std::cout << nicknames[startNodeId] << " -> ";
  for (int i = path.size() - 1; i >= 0; i--) {
    std::cout << nicknames[path[i]] << " -> ";
  }
  std::cout << nicknames[targetNodeId] << std::endl;
}

void check(Graph& graphObj, const std::vector<std::string>& nicknames,
           std::string startNickname, std::string targetNickname) {
  int startNodeId, targetNodeId;
  try {
    startNodeId = getNodeIdFromNickname(nicknames, startNickname);
    targetNodeId = getNodeIdFromNickname(nicknames, targetNickname);
  } catch (std::invalid_argument e) {  // If there is no matching nickname
    std::cerr << e.what() << std::endl;
    return;
  }
  // Set startNodeId and targetNodeId to the graph object
  graphObj.setStartTargetNode(startNodeId, targetNodeId);
  // Couduct breadth first search to find the shortest path
  graphObj.breadthFirstSearch();
  // If reachable, get the shortest path and print
  if (graphObj.ifReachable()) {
    std::vector<int> path = graphObj.getShortestPath();
    printPath(startNodeId, targetNodeId, path, nicknames);
  }
}

void runTest(Graph& graphObj, const std::vector<std::string>& nicknames) {
  check(graphObj, nicknames, "adrian", "hugh");   // Adrian and Me
  check(graphObj, nicknames, "adrian", "alan");   // can reach directory  (ALEXNOTE:  directly?)
  check(graphObj, nicknames, "adrian", "jack");   // can reach via some people
  check(graphObj, nicknames, "adrian", "betty");  // unreachable
  check(graphObj, nicknames, "hoge", "fuga");     // no such nickname
  check(graphObj, nicknames, "", "");             // empty input
 
  // ALEXNOTE: how about testing the same name as the source and dest?  does it work okay?
}

int main() {
  // Load nicknames from tsv file
  std::vector<std::string> nicknames =
      loadnicknames("./data/dummy_sns/nicknames.txt");
  int numOfNodes = nicknames.size();
  // Initialize graph object
  Graph graphObj(numOfNodes);
  graphObj.loadGraph("./data/dummy_sns/links.txt");
  std::cout << "loading finished" << std::endl;
  runTest(graphObj, nicknames);

  std::string startNickname, targetNickname;
  while (1) {
    std::cout << "Please input start and target nickname" << std::endl;
    std::cin >> startNickname >> targetNickname;
    check(graphObj, nicknames, startNickname, targetNickname);
  }
  return 0;
}
