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
      // graph_ is a vector of vector of pair, whose element is (node id,
      // adjacent node id).
      graph_[currentNodeId].push_back(adjacentNodeId);
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
    initializeVectors();
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

  void initializeVectors() {
    std::fill(visitStatus_.begin(), visitStatus_.end(), 0);
    std::fill(distance_.begin(), distance_.end(), VERY_LARGE_NUM);
    std::fill(parent_.begin(), parent_.end(), -1);
  }
};

// Load list of pages from tsv file.
std::vector<std::string> loadPages(std::string path_to_pages_file) {
  io::CSVReader<2, io::trim_chars<' '>, io::no_quote_escape<'\t'>> pages_reader(
      path_to_pages_file);
  int nodeId;
  std::string page_title;
  std::vector<std::string> pages;
  while (pages_reader.read_row(nodeId, page_title)) {
    pages.push_back(page_title);
  }
  return pages;
}

// Search pages and return the node id for the input page title.
int getNodeIdFromPageTitle(const std::vector<std::string>& pages,
                           std::string pageTitle) {
  auto iteratorOfTarget = find(pages.begin(), pages.end(), pageTitle);
  // If the input was not found, throw exception.
  if (iteratorOfTarget == pages.end()) {
    throw std::invalid_argument("No page title named '" + pageTitle + "'");
  }
  int nodeIdOfTarget = iteratorOfTarget - pages.begin();
  return nodeIdOfTarget;
}

// Print the formatted path.
void printPath(int startNodeId, int targetNodeId, std::vector<int>& path,
               const std::vector<std::string>& pages) {
  std::cout << pages[startNodeId] << " -> ";
  for (int i = path.size() - 1; i >= 0; i--) {
    std::cout << pages[path[i]] << " -> ";
  }
  std::cout << pages[targetNodeId] << std::endl;
}

void check(Graph& graphObj, const std::vector<std::string>& pages,
           std::string startPageTitle, std::string targetPageTitle) {
  int startNodeId, targetNodeId;
  try {
    startNodeId = getNodeIdFromPageTitle(pages, startPageTitle);
    targetNodeId = getNodeIdFromPageTitle(pages, targetPageTitle);
  } catch (std::invalid_argument e) {  // If there is no matching page
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
    printPath(startNodeId, targetNodeId, path, pages);
  }
}

void runTest(Graph& graphObj, const std::vector<std::string>& pages) {
  check(graphObj, pages, "Google", "Facebook");    // can reach directory
  check(graphObj, pages, "Google", "リラックマ");  // can reach via some pages
  check(graphObj, pages, "旧電車通り_(北九州市)",
        "ロデオ_(コープランド)");          // unreachable
  check(graphObj, pages, "hoge", "fuga");  // no such page
  check(graphObj, pages, "", "");          // empty input
}

int main() {
  // Load page titles from tsv file
  std::vector<std::string> pages = loadPages("./data/wikipedia/pages.txt");
  int numOfNodes = pages.size();
  // Initialize graph object
  Graph graphObj(numOfNodes);
  graphObj.loadGraph("./data/wikipedia/links.txt");
  std::cout << "loading finished" << std::endl;
  runTest(graphObj, pages);

  std::string startPageTitle, targetPageTitle;
  while (1) {
    std::cout << "Please input start and target page" << std::endl;
    std::cin >> startPageTitle >> targetPageTitle;
    check(graphObj, pages, startPageTitle, targetPageTitle);
  }
  return 0;
}