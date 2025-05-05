#include "../include/dataStructures.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> load_words(const std::string &filename) {
  std::vector<std::string> words;
  std::ifstream file(filename);
  if (file.is_open()) {
    std::string word;
    while (file >> word) {
      words.push_back(word);
    }
    file.close();
  } else {
    std::cerr << "Error: Could not open word list file '" << filename << "'"
              << std::endl;
  }
  return words;
}

std::string create_document_data(int doc_id,
                                 const std::vector<std::string> &tokens) {
  std::stringstream ss;
  ss << "    { \"doc_" << doc_id << ".txt\", {";
  for (size_t i = 0; i < tokens.size(); ++i) {
    ss << "\"" << tokens[i] << "\"";
    if (i < tokens.size() - 1) {
      ss << ", ";
    }
  }
  ss << "} }";
  return ss.str();
}

int main() {
  const std::string output_filename = "tokenDump.cpp";
  const std::string word_list_filename = "english_words.txt";
  const long long target_size_bytes = 1024LL * 1024LL * 16LL;
  const int avg_words_per_doc = 100;
  // Removed max_documents limit

  std::vector<std::string> word_list = load_words(word_list_filename);
  if (word_list.empty()) {
    return 1;
  }

  std::ofstream outfile(output_filename);
  if (!outfile.is_open()) {
    std::cerr << "Error: Could not open output file '" << output_filename << "'"
              << std::endl;
    return 1;
  }

  outfile << "#include \"../include/dataStructures.h\"\n\n";
  outfile << "std::vector<DocumentData> processedDocuments = {\n";

  std::mt19937 rng(std::time(0));
  std::uniform_int_distribution<int> word_dist(0, word_list.size() - 1);
  std::uniform_int_distribution<int> doc_length_dist(avg_words_per_doc / 2,
                                                     avg_words_per_doc * 2);

  long long current_size = outfile.tellp();
  int doc_count = 1;
  bool first_doc = true;

  while (current_size < target_size_bytes) {
    int num_words = doc_length_dist(rng);
    std::vector<std::string> tokens;
    for (int i = 0; i < num_words; ++i) {
      tokens.push_back(word_list[word_dist(rng)]);
    }
    std::string doc_data = create_document_data(doc_count, tokens);
    if (!first_doc) {
      outfile << ",\n";
    }
    outfile << doc_data;
    first_doc = false;
    current_size = outfile.tellp();
    doc_count++;

    if (doc_count % 1000 == 0) { // Output progress every 1000 documents
      std::cout << "Generated " << current_size / (1024 * 1024)
                << " MB so far..." << std::endl;
    }
  }

  outfile << "\n};\n";
  outfile.close();

  std::cout << "Generated '" << output_filename << "' with approximately "
            << current_size / (1024 * 1024) << " MB of data." << std::endl;
  std::cout << "Make sure you have a VERY large 'english_words.txt' file in "
               "the same directory."
            << std::endl;
  std::cout
      << "**WARNING: Compiling a C++ file of this size is highly impractical.**"
      << std::endl;

  return 0;
}