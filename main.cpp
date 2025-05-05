#include "./src/indexBuilder.cpp"
#include "./src/phraseSearch.cpp"
#include "include/dataStructures.h"

InvertedIndex Index;
DocumentMapping DocMap;
termDocTable TfIdf;

size_t getInvertedIndexSize(const InvertedIndex &index) {
  size_t totalSize = 0;

  for (const auto &pair : index) {
    totalSize += sizeof(
        pair.first); // Size of the std::string object (pointer, size, capacity)
    totalSize += pair.first.capacity(); // Actual allocated string buffer

    totalSize += sizeof(pair.second); // Size of the std::vector object
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
  buildIndex(DocMap, Index);
  std::vector<std::string> tkts{"a"};
  phraseSearch(tkts);
  std::cout << "InvertedIndex: " << getInvertedIndexSize(Index) << std::endl;
  std::cout << "DocumentMapping: " << sizeof(DocMap) << std::endl;
  std::cout << "TermDocTable: " << sizeof(TfIdf) << std::endl;
  int c;
  std::cin >> c;
  return 0;
}