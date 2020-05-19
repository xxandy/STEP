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

// Take string as an input and returns a vector (length: 26) that
// represents the frequency for each alphabet.
// Example: "aba" -> {2, 1, 0, ..., 0, 0}
std::vector<int> make_alphabet_vec(std::string &s) {
  int length_of_s = s.size();
  std::vector<int> div_vec(26);
  for (int i = 0; i < length_of_s; i++) {
    int idx = s[i] - 'a';
    div_vec[idx] += 1;
  }
  return div_vec;
}

int calc_score(std::vector<int> &v, int num_of_u_in_input) {
  // Vector representing the score which is added when the corresponding
  // alphabet is used.
  const std::vector<int> score_vec = {1, 1, 2, 1, 1, 2, 1, 2, 1, 3, 3, 2, 2,
                                      1, 1, 2, 3, 1, 1, 1, 1, 2, 2, 3, 2, 3};
  // If the number of additional 'u' (index 20) in v are more than that of 'q'
  // (index 16) in v, it is an invalid answer.
  if (v[16] < v[20] - num_of_u_in_input) {
    return 0;
  }
  // Additional 'u' must be omitted before calculating score.
  v[16] -= v[20];
  // Calculate the score.
  int sum = 1;  // including bonus
  for (unsigned int i = 0; i < v.size(); i++) {
    if (v[i] > 0) {
      sum += v[i] * score_vec[i];
    }
  }
  return sum * sum;
}

int main() {
  // Input is a string (length: 16) and is provided like "yrafnvicgbdohyiu".
  std::string input;
  std::cin >> input;

  // Make a std::vector that represents the frequency of alphabets from input.
  std::vector<int> input_alphabet_vec = make_alphabet_vec(input);
  // If 'q' (index 16) is in the input, 'u' (index 20) is added.
  int num_of_u_in_input = input_alphabet_vec[20];
  input_alphabet_vec[20] += input_alphabet_vec[16];

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
    // A word whose length is more than that of the input is excluded.
    if (str.size() > 16) {
      continue;
    }
    transform(str.cbegin(), str.cend(), str.begin(), ::tolower);
    // A word which is identical to the input is excluded.
    if (str != input) {
      dic.insert(str);
    }
  }

  // Make a vector that represents the frequency of alphabets for each word
  // in dic and store them in dic_alphabet. The elements of the dic_alphabet are
  // like {"aba", {2, 1, 0, ..., 0}}.
  int num_of_word_in_dic = dic.size();
  std::vector<std::pair<std::string, std::vector<int>>> dic_alphabet(
      num_of_word_in_dic);
  std::vector<int> tmp;
  int count = 0;
  for (std::string word : dic) {
    tmp = make_alphabet_vec(word);
    dic_alphabet[count].first = word;
    dic_alphabet[count].second = tmp;
    count++;
  }

  // Iterate through dic_alphabet and find a word that satisfies the
  // condition, which means that all elements in the alphabet frequency
  // vector are equal to or less than corresponding elements of
  // input_alphabet_vec.
  // Score is calculated for each hit to find a word which produces maximam one.
  int score;
  int max_score = 0;
  int max_idx = 0;
  for (unsigned int i = 0; i < dic_alphabet.size(); i++) {
    std::vector<int> alphabet_vec = dic_alphabet[i].second;
    bool if_partial_anagram =
        equal(input_alphabet_vec.begin(), input_alphabet_vec.end(),
              alphabet_vec.begin(), alphabet_vec.end(),
              [](int x, int y) { return x >= y; });
    if (if_partial_anagram) {
      score = calc_score(alphabet_vec, num_of_u_in_input);
      if (score > max_score) {
        max_score = score;
        max_idx = i;
      }
    }
  }

  if (max_score != 0) {
    std::cout << dic_alphabet[max_idx].first << std::endl;
  } else {
    std::cout << "NOT FOUND" << std::endl;
  }

  return 0;
}