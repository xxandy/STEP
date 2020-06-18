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
        dp_((1LL << N), std::vector<double>(N, inf)),
        pos_((1LL << N), std::vector<int>(N, inf)){};

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

  // Get the shorest tour by bitDP (bit dynamic programming).
  void SolveByBitDP() {
    // The start point (index 0) is set as unvisited.
    dp_[(1 << N_) - 1][0] = 0;
    for (long long tmp = (1 << N_) - 2; tmp >= 0; tmp--) {
      // S (bitset) is a set of points that are already visited.
      std::bitset<max_N> S(tmp); 
      // ALEXNOTE: is a bit set more performant than just an array?  (of course, the arry uses more space,
      //        but it may not be significant here.
      for (int now = 0; now < N_; now++) {
        // if now is not in S, skip.
        if (!S.test(now) && now != 0) continue;
        // Search the best next node from points outside S.
        for (int next = 0; next < N_; next++) {
          if (!S.test(next)) {
            double new_dist = dp_[tmp | (1 << next)][next] + dist_[now][next];
            if (new_dist < dp_[tmp][now]) {
              dp_[tmp][now] = new_dist;
              // update pos_ and record tour to restore it later.
              pos_[tmp][now] = next;
            }
          }
        }
      }
    }
  }

  // Return the shortest path length after conducting bitDP.
  double GetShortestLength() { return dp_[0][0]; }

  // Restore shortest tour after conducting bitDP and return it.
  std::vector<int> GetShortestTour() {
    int tmp = 0;
    int now = 0;
    std::vector<int> shortest_tour(1, 0);
    for (int i = 0; i < N_; i++) {
      int next = pos_[tmp][now];
      shortest_tour.push_back(next);
      tmp = (tmp | (1 << next));
      now = next;
    }
    return shortest_tour;
  }

 private:
  int N_;
  std::vector<std::vector<double>> coordinates_;
  std::vector<std::vector<double>> dist_;
  std::vector<std::vector<double>> dp_;
  std::vector<std::vector<int>> pos_;
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
  // ex) input_0.csv -> bitDP_0.csv
  std::string output_file_path;
  if (a.exist("output")) {
    output_file_path = a.get<std::string>("output");
  } else {
    output_file_path =
        "./output/" + GetOutputFileName(input_file_path, "bitDP");
  }

  std::vector<std::vector<double>> coordinates =
      LoadCoordinates(input_file_path);
  int N = coordinates.size();
  TSP tsp(N);
  tsp.SetCoordinates(coordinates);
  tsp.CalcDistance();
  tsp.SolveByBitDP();
  double shortest_length = tsp.GetShortestLength();
  std::vector<int> shortest_tour = tsp.GetShortestTour();
  std::cout << shortest_length << std::endl;
  OutputTourToFile(shortest_tour, output_file_path);
}
