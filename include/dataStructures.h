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
  size_t operator()(const pair<string, int> &p) const {
    return hash<string>()(p.first) ^ (hash<int>()(p.second) << 1);
  }
};
} // namespace std

struct BoundingBox {
  int left;
  int top;
  int right;
  int bottom;
};

struct PositionInfo {
  int tokenIndex;
  BoundingBox bbox;
  int pageNumber; // Add page number
};
struct Posting {
  int docID;
  std::vector<PositionInfo> positions;
};

using InvertedIndex = std::unordered_map<std::string, std::vector<Posting>>;
using DocumentMapping = std::vector<std::string>;
using searchResults = std::unordered_map<int, std::vector<std::vector<int>>>;
using termDocTable = std::unordered_map<std::pair<std::string, int>, int>;

struct TokenInfo {
  std::string text;
  poppler::rectf boundingBox;
};

#endif // DATA_STRUCTURES_H