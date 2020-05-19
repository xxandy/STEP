/*
 * Author  : Ayako Iwasaki
 * Project : STEP2020
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

// Find anagram by binary search and return it.
std::string binary_search_anagram(
    std::string sorted_input,
    std::vector<std::pair<std::string, std::string>>& sorted_dic) {
  int dic_size = sorted_dic.size();
  int left_idx = 0;
  int right_idx = dic_size;
  int middle_idx;
  std::string middle;
  while (left_idx < right_idx) {
    middle_idx = (left_idx + right_idx) / 2;
    middle = sorted_dic[middle_idx].first;
    if (middle == sorted_input) {
      return sorted_dic[middle_idx].second;
    } else if (sorted_input < middle) {
      right_idx = middle_idx;
    } else {
      left_idx = middle_idx + 1;
    }
  }
  return "NOT FOUND";
}

int main() {
  std::string input;
  std::cin >> input;
  std::transform(input.cbegin(), input.cend(), input.begin(), ::tolower);
  // Sort input to make sorted_input.
  std::string sorted_input = input;
  std::sort(sorted_input.begin(), sorted_input.end());
  unsigned int length_of_input = input.size();

  // Load dictionary.
  // Words in dictionary are added to the unordered_set dic.
  std::ifstream ifs("./dic.txt");
  std::string str;
  std::unordered_set<std::string> dic;
  if (ifs.fail()) {
    std::cerr << "Failed to open file." << std::endl;
    return -1;
  }
  while (getline(ifs, str)) {
    // A word whose length is not equal to that of the input is excluded.
    if (str.size() != length_of_input) {
      continue;
    }
    std::transform(str.cbegin(), str.cend(), str.begin(), ::tolower);
    // A word which is identical to the input is excluded.
    if (str != input) {
      dic.insert(str);
    }
  }

  // Sort words in dic respectively and store them in sorted_dic.
  // The elments of the sorted_dic are like {sorted_word, original_word}.
  int num_of_word_in_dic = dic.size();
  std::vector<std::pair<std::string, std::string>> sorted_dic(
      num_of_word_in_dic);
  int count = 0;
  std::string tmp;
  for (std::string word : dic) {
    tmp = word;
    std::sort(tmp.begin(), tmp.end());
    sorted_dic[count].first = tmp;
    sorted_dic[count].second = word;
    count++;
  }

  // Sort elements of the sorted_dic by its first element, namely, sorted_word.
  std::sort(sorted_dic.begin(), sorted_dic.end(),
            [](const auto& x, const auto& y) {
              if (x.first == y.first) {
                return x.second < y.second;
              } else {
                return x.first < y.first;
              }
            });

  // Search anagram by binary search.
  std::string result = binary_search_anagram(sorted_input, sorted_dic);
  std::cout << result << std::endl;
  return 0;
}