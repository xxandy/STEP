/*
 * Author  : Ayako Iwasaki
 * Project : STEP2020
 */

#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

// Create a square matrix whose elements are random numbers between 0 and 1000
std::vector<std::vector<int>> create_random_square_matrix(int N, int seed) {
  std::mt19937 mt(seed);
  std::vector<std::vector<int>> matrix(N, std::vector<int>(N));
  std::uniform_int_distribution<int> dist(0, 1000);
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      matrix[i][j] = dist(mt);
    }
  }
  return matrix;
}

// Calculate the product of two square matrixes
std::vector<std::vector<int>> calc_prod_matrix(
    const std::vector<std::vector<int>> &A,
    const std::vector<std::vector<int>> &B) {
  int N = A.size();
  int sum;
  std::vector<std::vector<int>> result(N, std::vector<int>(N));
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      sum = 0;
      for (int k = 0; k < N; k++) {
        sum += A[i][k] * B[k][j];
      }
      result[i][j] = sum;
    }
  }
  return result;
}

int main() {
  std::ofstream ofs("result.tsv");
  for (int N = 100; N <= 1000; N += 100) {
    std::vector<std::vector<int>> A = create_random_square_matrix(N, 0);
    std::vector<std::vector<int>> B = create_random_square_matrix(N, 1);

    auto start = std::chrono::system_clock::now();
    std::vector<std::vector<int>> result = calc_prod_matrix(A, B);
    auto end = std::chrono::system_clock::now();

    double elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    ofs << N << '\t' << elapsed << std::endl;
  }
  return 0;
}