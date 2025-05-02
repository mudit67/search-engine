#include "./bin/tokenDump.cpp"
#include <algorithm>
#include <ostream>
#include <set> // Using set temporarily within the value can simplify uniqueness handling
#include <string>
#include <unordered_map>
#include <vector>
struct Posting {
  int docID;
  int frequency;
};
// Type alias for our main index structure
// Token as the Key and the <DocumentID,Frequency> as the value.
using InvertedIndex = std::unordered_map<std::string, std::vector<Posting>>;

// Type alias for mapping document IDs back to their original identifiers (e.g.,
// filenames)
using DocumentMapping = std::vector<std::string>;

void printIndex(const InvertedIndex &index, const DocumentMapping &docMapping) {
  std::cout << "--- Inverted Index ---" << std::endl;
  for (const auto &pair : index) {
    std::cout << "'" << pair.first << "': [";
    bool first = true;
    for (const auto &idFreqPair : pair.second) {
      if (!first)
        std::cout << ", ";
      std::cout << idFreqPair.docID << " : "
                << idFreqPair.frequency; // Print doc ID
      // Optionally print the filename too:
      if (idFreqPair.docID >= 0 && idFreqPair.docID < docMapping.size()) {
        std::cout << " (" << docMapping[idFreqPair.docID] << ")";
      }
      first = false;
    }
    std::cout << "]" << std::endl;
  }
  std::cout << "----------------------" << std::endl;
  std::cout << "\n--- Document Mapping ---" << std::endl;
  for (int i = 0; i < docMapping.size(); ++i) {
    std::cout << "ID " << i << ": " << docMapping[i] << std::endl;
  }
  std::cout << "------------------------" << std::endl;
}

std::vector<Posting>::iterator searchDocId(std::vector<Posting> &PostingArr,
                                           int docId) {

  auto it = std::lower_bound(
      PostingArr.begin(), PostingArr.end(), docId,
      [](const Posting &p, int targetDocId) { return p.docID < targetDocId; });

  return it;
}

void insert_sorted(std::vector<Posting> &vec, Posting element,
                   std::vector<Posting>::iterator iter) {
  vec.insert(iter, element);
}

int main() {
  DocumentMapping DocMap;
  InvertedIndex Index;

  for (int i = 0; i < processedDocuments.size(); i++) {
    DocMap.push_back(processedDocuments[i].id);
    for (int j = 0; j < processedDocuments[i].tokens.size(); j++) {
      std::string token = processedDocuments[i].tokens[j];
      auto docIdIter = searchDocId(Index[token], i);
      if (docIdIter == Index[token].end() || docIdIter->docID != i) {
        // Index[token].push_back({i, 1});
        insert_sorted(Index[token], {i, 1}, docIdIter);
      } else {
        // Index[token][docIdIter].frequency++;
        docIdIter->frequency++;
      }
    }
  }
  std::cout << "Index Built!" << std::endl;
  printIndex(Index, DocMap);
}