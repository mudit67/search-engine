#include "./src/indexBuilder.cpp"
#include "./src/phraseSearch.cpp"
#include "include/dataStructures.h"
#include "utils/pdf_file_path.cpp"
InvertedIndex Index;
DocumentMapping DocMap;
termDocTable TfIdf;
size_t getInvertedIndexSize(const InvertedIndex &index) {
  size_t totalSize = 0;
  for (const auto &pair : index) {
    totalSize += sizeof(
        pair.first); // Size of the std::string object (pointer, size, capacity)
    totalSize += pair.first.capacity(); // Actual allocated string buffer
    totalSize += sizeof(pair.second);   // Size of the std::vector object
                                        // (pointer, size, capacity)
    totalSize += pair.second.capacity() *
                 sizeof(Posting); // Allocated space for Postings
    for (const auto &posting : pair.second) {
      totalSize += sizeof(posting.docID);
      totalSize += sizeof(posting.positions); // Size of the std::vector object
      totalSize += posting.positions.capacity() *
                   sizeof(int); // Allocated space for positions
    }
  }
  return totalSize;
}
int main() {
  std::vector<std::string> pdfFiles = getAllPdfFiles("./bin");
  // std::vector<std::string> pdfFiles{"./bin/thinkpython2.pdf"};
  for (const auto &pdfPath : pdfFiles) {
    std::cout << "Processing PDF: " << pdfPath << std::endl;
    processPdfAndBuildIndex(pdfPath);
  }

  std::string query;
  std::cout
      << "Enter your search query (space-separated words, or -1 to exit): ";

  while (std::getline(std::cin, query) && query != "-1") {
    std::istringstream iss(query);
    std::vector<std::string> tokensToSearch;
    std::string token;
    while (iss >> token) {
      token.erase(std::remove_if(token.begin(), token.end(), ispunct),
                  token.end());
      std::transform(token.begin(), token.end(), token.begin(), ::tolower);
      tokensToSearch.push_back(token);
    }

    if (!tokensToSearch.empty()) {
      std::cout << "Searching for: ";
      for (const auto &t : tokensToSearch) {
        std::cout << "\"" << t << "\" ";
      }
      std::cout << std::endl;
      phraseSearch(tokensToSearch);
    } else {
      std::cout << "No search query entered." << std::endl;
    }

    std::cout << "\nEnter your next search query (space-separated words, or -1 "
                 "to exit): ";
  }

  std::cout << "Exiting search." << std::endl;
  return 0;
}