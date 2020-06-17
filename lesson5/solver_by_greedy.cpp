#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wshadow"
#include "cmdline.h"
#include "csv.h"
#pragma GCC diagnostic pop

#include <bitset>
#include <cmath>
#include <fstream>
#include <iostream>
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
        tour_(1, 0){};

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
      tour_.push_back(next);
      now = next;
    }
    tour_.push_back(0);
  }

  // Take tour as an input and return the path length.
  double CalcScoreFromTour(const std::vector<int>& tour) {
    double result = 0;
    for (int i = 1; i < N_ + 1; i++) {
      result += dist_[tour[i - 1]][tour[i]];
    }
    return result;
  }

  // Return the shortest path length after conducting greedy algorithm.
  double GetShortestLength() { return CalcScoreFromTour(tour_); }

  // Restore shortest tour after conducting greedy algorithm and return it.
  std::vector<int> GetShortestTour() { return tour_; }

 private:
  int N_;
  std::vector<std::vector<double>> coordinates_;
  std::vector<std::vector<double>> dist_;
  std::vector<int> tour_;
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
    output_file_path =
        "./output/" + GetOutputFileName(input_file_path, "greedy");
  }

  std::vector<std::vector<double>> coordinates =
      LoadCoordinates(input_file_path);
  int N = coordinates.size();
  TSP tsp(N);
  tsp.SetCoordinates(coordinates);
  tsp.CalcDistance();
  tsp.SolveByGreedy();
  double shortest_length = tsp.GetShortestLength();
  std::vector<int> shortest_tour = tsp.GetShortestTour();
  std::cout << shortest_length << std::endl;
  OutputTourToFile(shortest_tour, output_file_path);
}
