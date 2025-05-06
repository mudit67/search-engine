#include "../include/dataStructures.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <vector>

#define SEARCH_TOP_N_DOCS 3
#define PROXIMITY_THRESHOLD 100 // Adjust as needed

extern InvertedIndex Index;
extern DocumentMapping DocMap;
extern termDocTable TfIdf;

// Helper function for comparing Posting objects by docID
bool docIdComp(const Posting &a, const Posting &b) {
  return (a.docID < b.docID);
}

// Helper function to check if two bounding boxes are adjacent
bool areAdjacent(const BoundingBox &bbox1, const BoundingBox &bbox2,
                 double proximityThreshold,
                 double verticalProximityThreshold = 6.0) {
  // Check for horizontal adjacency
  bool horizontal = std::abs(bbox2.left - bbox1.right) < proximityThreshold ||
                    std::abs(bbox1.left - bbox2.right) < proximityThreshold;

  // Check for vertical overlap
  bool verticalOverlap =
      std::max(bbox1.top, bbox2.top) < std::min(bbox1.bottom, bbox2.bottom);

  // Check for close vertical proximity (if no overlap)
  bool verticalProximity =
      std::abs(bbox1.top - bbox2.bottom) < verticalProximityThreshold ||
      std::abs(bbox2.top - bbox1.bottom) < verticalProximityThreshold;
  // std::cout << horizontal << " " << verticalOverlap << " " <<
  // verticalProximity
  //           << std::endl;
  return horizontal && (verticalOverlap || verticalProximity);
}
// Function to search for a phrase in a specific document, considering proximity
void searchInDocWithProximity(
    int docId, const std::vector<std::string> &tokensToSearch,
    std::vector<int> &docResults, // Store only token indices
    double proximityThreshold, int tokenDistanceThreshold) {
  std::vector<const std::vector<PositionInfo> *> termPositions;
  Posting temp;
  temp.docID = docId;

  for (const auto &token : tokensToSearch) {
    auto it = std::lower_bound(
        Index[token].begin(), Index[token].end(), temp,
        [](const Posting &a, const Posting &b) { return a.docID < b.docID; });
    if (it == Index[token].end() || it->docID != docId) {
      std::cout << "  Token \"" << token << "\" not found in document " << docId
                << std::endl;
      return;
    }
    termPositions.push_back(&it->positions);
  }

  if (tokensToSearch.size() == 1) {
    for (const auto &posInfo : *termPositions[0]) {
      docResults.push_back(posInfo.tokenIndex);
    }
    return;
  }

  std::vector<int> currentMatchIndices;
  std::function<void(int, const BoundingBox &, int, int)> findNext =
      [&](int termIndex, const BoundingBox &lastBBox, int lastTokenIndex,
          int lastPageNumber) {
        if (termIndex == tokensToSearch.size()) {
          docResults.insert(docResults.end(), currentMatchIndices.begin(),
                            currentMatchIndices.end()); // Store token indices
          return;
        }

        for (const auto &posInfo : *termPositions[termIndex]) {
          BoundingBox currentBBox = posInfo.bbox;
          int currentTokenIndex = posInfo.tokenIndex;
          int currentPageNumber = posInfo.pageNumber;
          if (termIndex == 0 ||
              (areAdjacent(lastBBox, currentBBox, proximityThreshold) &&
               currentTokenIndex > lastTokenIndex &&
               (currentTokenIndex - lastTokenIndex) <= tokenDistanceThreshold &&
               currentPageNumber == lastPageNumber)) {
            currentMatchIndices.push_back(currentTokenIndex);
            findNext(termIndex + 1, currentBBox, currentTokenIndex,
                     currentPageNumber);
            currentMatchIndices.pop_back();
          }
        }
      };

  if (!termPositions.empty() && !termPositions[0]->empty()) {
    for (const auto &firstTermPos : *termPositions[0]) {
      currentMatchIndices.push_back(firstTermPos.tokenIndex);
      findNext(1, firstTermPos.bbox, firstTermPos.tokenIndex,
               firstTermPos.pageNumber);
      currentMatchIndices.pop_back();
    }
  }
}

void printSearchResults(
    const std::unordered_map<int, std::vector<int>> &results,
    const std::vector<std::string> &queryTokens) {
  std::cout << "--- Search Results ---" << std::endl;
  if (results.empty()) {
    std::cout << "No search results found." << std::endl;
    return;
  }

  for (const auto &docIdOccurrencesPair : results) {
    int docId = docIdOccurrencesPair.first;
    const std::vector<int> &occurrences = docIdOccurrencesPair.second;

    std::cout << "Document: " << DocMap[docId] << std::endl;
    std::cout << "  Found occurrence(s) at token indices: ";

    for (int tokenIndex : occurrences) {
      std::cout << tokenIndex << " ";

      // Find the page number for this tokenIndex
      // std::cout << "searching page of: " << queryTokens[0] << std::endl;
      for (const auto &posting : Index.at(queryTokens[0])) {
        // std::cout << "searching page of: " << queryTokens[0] << std::endl;
        if (posting.docID == docId) {
          for (const auto &posInfo : posting.positions) {
            if (posInfo.tokenIndex == tokenIndex) {
              std::cout << " (Page: " << posInfo.pageNumber << ") "
                        << std::endl;
              break;
            }
          }
          break;
        }
      }
    }
    std::cout << std::endl;
  }
  std::cout << "----------------------" << std::endl;
}

bool phraseSearch(std::vector<std::string> &tokensToSearch) {
  if (DocMap.empty()) {
    std::cout << "No documents indexed yet." << std::endl;
    return false;
  }

  std::vector<std::pair<double, int>> docScores;
  for (size_t i = 0; i < DocMap.size(); ++i) {
    double score = 0.0;
    for (const auto &token : tokensToSearch) {
      if (TfIdf.count({token, static_cast<int>(i)})) {
        score += TfIdf[{token, static_cast<int>(i)}];
      }
    }
    docScores.push_back({score, static_cast<int>(i)});
  }

  std::sort(docScores.rbegin(), docScores.rend());

  std::unordered_map<int, std::vector<int>> searchResults;
  for (const auto &docScorePair : docScores) {
    if (searchResults.size() >= SEARCH_TOP_N_DOCS && SEARCH_TOP_N_DOCS > 0) {
      break;
    }
    std::vector<int> docResults; // Results for the current document
    searchInDocWithProximity(docScorePair.second, tokensToSearch, docResults,
                             PROXIMITY_THRESHOLD, 30);
    if (!docResults.empty()) {
      searchResults[docScorePair.second] = docResults;
    }
    if (docScorePair.first == 0.0 && searchResults.empty()) {
      break; // No more relevant documents
    }
  }

  printSearchResults(searchResults, tokensToSearch);
  return !searchResults.empty();
}