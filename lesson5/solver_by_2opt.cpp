#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wshadow"
#include "cmdline.h"
#include "csv.h"
#pragma GCC diagnostic pop

#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>

class TSP {
 public:
  static constexpr double inf = 1e8;
  static constexpr int max_N = 20;
  TSP(int N)
      : N_(N),
        coordinates_(N, std::vector<double>(N)),
        dist_(N, std::vector<double>(N)),
        current_best_tour_(1, 0),
        tmp_tour_(N + 1){};

  // Take matrix of coodinates as an input and set it to coordinates_.
  void SetCoordinates(const std::vector<std::vector<double>>& coordinates) {
    coordinates_ = coordinates;
  }

  // Calculate eucledian distance between every point and save it to dist_.
  void CalcDistance() {
    for (int i = 0; i < N_; i++) {
      for (int j = i + 1; j < N_; j++) {
        double distance = sqrt(pow(coordinates_[i][0] - coordinates_[j][0], 2) +
                               pow(coordinates_[i][1] - coordinates_[j][1], 2));
        dist_[i][j] = distance;
        dist_[j][i] = distance;
      }
    }
  }

  // Get the shorest tour by greedy algorithm.
  void SolveByGreedy() {
    std::set<int> not_visited;
    for (int i = 1; i < N_; i++) {
      not_visited.insert(i);
    }
    int now = 0;
    while (not_visited.size()) {
      double tmp = inf;
      int next = 0;
      for (auto next_candidate : not_visited) {
        if (tmp > dist_[now][next_candidate]) {
          tmp = dist_[now][next_candidate];
          next = next_candidate;
        }
      }
      not_visited.erase(next);
      current_best_tour_.push_back(next);
      now = next;
    }
    current_best_tour_.push_back(0);
  }

  // Take tour as an input and return the path length.
  double CalcScoreFromTour(const std::vector<int>& tour) {
    double result = 0;
    for (int i = 1; i < N_ + 1; i++) {
      result += dist_[tour[i - 1]][tour[i]];
    }
    return result;
  }

  // Return the shortest path length by evaluating current_best_tour_.
  double GetShortestLength() { return CalcScoreFromTour(current_best_tour_); }

  // Return shortest tour.
  std::vector<int> GetShortestTour() { return current_best_tour_; }

  // Optimize a tour by 2-opt algorithm (swap two edges until there is no
  // intersection).
  void OptimizeBy2Opt() {
    int score = CalcScoreFromTour(current_best_tour_);
    double new_score;
    tmp_tour_ = current_best_tour_;
    int num_of_crossed_edge = inf;

    int cnt = 0;
    while (num_of_crossed_edge != 0 && cnt != 10) {
      num_of_crossed_edge = 0;
      // Iterate through all edges.
      for (int idx1 = 0; idx1 <= N_ - 3; idx1++) {
        for (int idx2 = idx1 + 2; idx2 <= N_ - 1; idx2++) {
          // Check if edge (tour[idx1], tour[idx + 1]) and (tour[idx2],
          // tour[idx2 + 1]) has an intersection. If not, skip.
          if (!IsTwoEdgesCrossed(idx1, idx2)) continue;
          num_of_crossed_edge++;
          // Swap two edges and update score if the score gets better.
          std::vector<int> new_tour = SwapTwoEdges(idx1, idx2, tmp_tour_);
          new_score = CalcScoreFromTour(new_tour);
          if (new_score < score) {
            tmp_tour_ = new_tour;
            score = new_score;
          }
        }
      }
      cnt++;
    }
    current_best_tour_ = tmp_tour_;
  }

 private:
  // Get the relationship between a line (connectiong point p and point q) and a
  // point x. Return a positive value if x, p, q is in counter clockwise order,
  // negative value if they are clockwise order,
  // zero if x is on line connecting p with q.
  double GetPointLineRelationship(int x, int p, int q) {
    double result = (coordinates_[p][0] - coordinates_[q][0]) *
                        (coordinates_[x][1] - coordinates_[p][1]) +
                    (coordinates_[p][1] - coordinates_[q][1]) *
                        (coordinates_[p][0] - coordinates_[x][0]);
    return result;
  }

  // Return 1 when
  // - an edge connecting tour[idx1] and tour[idx1 + 1]
  // and
  // - an edge connecting tour[idx2] and tour[idx2 + 1]
  // has a intersection. Otherwise return 0.
  bool IsTwoEdgesCrossed(int idx1, int idx2) {
    int a = tmp_tour_[idx1];
    int b = tmp_tour_[idx1 + 1];
    int c = tmp_tour_[idx2];
    int d = tmp_tour_[idx2 + 1];
    bool is_cd_cross_ab =
        (GetPointLineRelationship(c, a, b) * GetPointLineRelationship(d, a, b) <
         0);
    bool is_ab_cross_cd =
        (GetPointLineRelationship(a, c, d) * GetPointLineRelationship(b, c, d) <
         0);
    return (is_cd_cross_ab & is_ab_cross_cd);
  }

  // Reverse all elements of vector tour between idx and idx2 to swap two edges.
  // Return a new tour vector.
  std::vector<int> SwapTwoEdges(int idx1, int idx2,
                                const std::vector<int>& tour) {
    std::vector<int> new_tour = tour;
    for (int i = idx1 + 1; i <= idx2; i++) {
      new_tour[i] = tmp_tour_[idx2 + idx1 + 1 - i];
    }
    return new_tour;
  }

  int N_;
  std::vector<std::vector<double>> coordinates_;
  std::vector<std::vector<double>> dist_;
  std::vector<int> current_best_tour_;
  std::vector<int> tmp_tour_;
};

// Load coodinates from input file.
std::vector<std::vector<double>> LoadCoordinates(std::string input_file_path) {
  io::CSVReader<2> reader(input_file_path);
  std::vector<std::vector<double>> coordinates;
  reader.read_header(io::ignore_extra_column, "x", "y");
  double x, y;
  while (reader.read_row(x, y)) {
    coordinates.push_back({x, y});
  }
  return coordinates;
}

// Create output file name from input file path and prefix
// if no output file path is provided.
// ex) input_0.csv -> ${prefix}_0.csv
std::string GetOutputFileName(std::string input_file_path, std::string prefix) {
  // Extract file name from input file path.
  std::string input_file_name = input_file_path;
  std::string::size_type slash_pos = input_file_path.rfind("/");
  if (slash_pos != std::string::npos) {
    input_file_name = input_file_path.substr(
        slash_pos + 1, input_file_path.size() - slash_pos - 1);
  }
  // Check if input file name is valid.
  std::string::size_type input_pos = input_file_name.find("input_");
  if (input_pos == std::string::npos) {
    throw std::invalid_argument("Invalid input file name");
  }
  // Create output file name.
  std::string output_file_name =
      prefix + "_" +
      input_file_name.substr(input_pos + 6, input_file_name.size());
  return output_file_name;
}

// Write a tour to a file.
void OutputTourToFile(const std::vector<int>& tour,
                      std::string path_to_output_file) {
  std::string result_str = "index\n";
  int tour_size = tour.size();
  for (int i = 0; i < tour_size - 1; i++) {
    result_str += std::to_string(tour[i]) + "\n";
  }
  std::ofstream outputfile(path_to_output_file);
  outputfile << result_str;
  outputfile.close();
}

int main(int argc, char* argv[]) {
  // Get input and output file path from command line.
  cmdline::parser a;
  // --input is mandetory.
  a.add<std::string>("input", 'i', "input file path", true, "");
  // --output is optional.
  a.add<std::string>("output", 'o', "output file path", false, "");
  a.parse_check(argc, argv);
  std::string input_file_path = a.get<std::string>("input");

  // If no output file path is provided, create it from input file path.
  // ex) input_0.csv -> greedy_0.csv
  std::string output_file_path;
  if (a.exist("output")) {
    output_file_path = a.get<std::string>("output");
  } else {
    output_file_path = "./output/" + GetOutputFileName(input_file_path, "2opt");
  }

  std::vector<std::vector<double>> coordinates =
      LoadCoordinates(input_file_path);
  int N = coordinates.size();
  TSP tsp(N);
  tsp.SetCoordinates(coordinates);
  tsp.CalcDistance();

  // First, solve by greedy algorithm.
  tsp.SolveByGreedy();
  double shortest_length_after_greedy = tsp.GetShortestLength();
  std::vector<int> shortest_tour_after_greedy = tsp.GetShortestTour();
  std::cout << shortest_length_after_greedy << std::endl;

  // Next, swap edges to clear intersections by 2-opt algorithm.
  tsp.OptimizeBy2Opt();
  double shortest_length_after_2Opt = tsp.GetShortestLength();
  std::vector<int> shortest_tour_after_2Opt = tsp.GetShortestTour();
  std::cout << shortest_length_after_2Opt << std::endl;

  OutputTourToFile(shortest_tour_after_2Opt, output_file_path);
}
