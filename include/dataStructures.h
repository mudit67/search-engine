#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H
#include <algorithm>
#include <functional>
#include <iostream>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-global.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include <poppler/cpp/poppler-page.h>
#include <string>
#include <unordered_map>
#include <vector>
namespace std {
template <> struct hash<pair<string, int>> {
  size_t operator()(const pair<string, int> &p)
      const { // Use a combination of the hashes of the individual elements
    // You can use different combining methods
    return hash<string>()(p.first) ^ (hash<int>()(p.second) << 1);
  }
};
} // namespace std
// Represents the output of your (future) PDF Parser + Tokenizer

struct DocumentData {
  std::string id; // e.g., filename or path
  std::vector<std::string> tokens;
};
struct Posting {
  int docID; // int frequency;
  std::vector<int> positions;
}; // Type alias for
   // our main index structure
// Token as the Key and the <DocumentID,Frequency> as the value.
using InvertedIndex =
    std::unordered_map<std::string,
                       std::vector<Posting>>; // Type alias for mapping document
                                              // IDs back to their original
                                              // identifiers (e.g.,// filenames)
using DocumentMapping = std::vector<std::string>;
using searchResults =
    std::unordered_map<int,
                       std::vector<std::vector<int>>>; // Term,DocId to int.
using termDocTable = std::unordered_map<std::pair<std::string, int>, int>;
struct TokenInfo {
  std::string text;
  poppler::rectf boundingBox;
};

#endif // DATA_STRUCTURES_H