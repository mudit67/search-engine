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

  // std::vector<std::string> pdfPaths{"./bin/btee.pdf"};
  std::vector<std::string> pdfPaths = getAllPdfFiles();
  for (auto pdfPath : pdfPaths) {
    std::cout << "Indexing: " << pdfPath << std::endl;
    processPdfAndBuildIndex(pdfPath);
  }
  // printIndex();

  std::vector<std::string> tkts{"data", "applications"};
  phraseSearch(tkts);
  // std::cout << "InvertedIndex: " << getInvertedIndexSize(Index) << std::endl;
  // std::cout << "DocumentMapping: " << sizeof(DocMap) << std::endl;
  // std::cout << "TermDocTable: " << sizeof(TfIdf) << std::endl;
  int c;
  std::cin >> c;
  return 0;
}