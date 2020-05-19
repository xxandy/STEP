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
std::vector<int> make_alphabet_vec(std::string& s) {
  int length_of_s = s.size();
  std::vector<int> div_vec(26);
  for (int i = 0; i < length_of_s; i++) {
    int idx = s[i] - 'a';
    div_vec[idx] += 1;
  }
  return div_vec;
}

int main() {
  std::string input;
  std::cin >> input;
  unsigned int length_of_input = input.size();
  transform(input.cbegin(), input.cend(), input.begin(), ::tolower);

  // Make a vector that represents the frequency of alphabets from input.
  std::vector<int> input_alphabet_vec = make_alphabet_vec(input);

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
    if (str.size() > length_of_input) {
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
  for (unsigned int i = 0; i < dic_alphabet.size(); i++) {
    std::vector<int> alphabet_vec = dic_alphabet[i].second;
    bool if_partial_anagram =
        equal(input_alphabet_vec.begin(), input_alphabet_vec.end(),
              alphabet_vec.begin(), alphabet_vec.end(),
              [](int x, int y) { return x >= y; });
    if (if_partial_anagram) {
      std::cout << dic_alphabet[i].first << std::endl;
      return 0;
    }
  }
  std::cout << "NOT FOUND" << std::endl;
  return 0;
}