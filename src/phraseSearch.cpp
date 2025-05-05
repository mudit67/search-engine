#include "../include/dataStructures.h"
#include <algorithm>
#include <bits/stl_numeric.h>
#define searchTopNDocs 3
// #include "./termRank.cpp"

extern InvertedIndex Index;
extern DocumentMapping DocMap;
extern termDocTable TfIdf;

bool docIdComp(const Posting &a, const Posting &b) {
  return (a.docID < b.docID);
}
void printSearchResults(
    const std::unordered_map<int, std::vector<std::vector<int>>> &results) {
  std::cout << "--- Search Results ---" << std::endl;
  if (results.empty()) {
    std::cout << "No search results found." << std::endl;
    return;
  }

  for (const auto &pair : results) {
    int docId = pair.first;
    const std::vector<std::vector<int>> &allOccurrences = pair.second;

    std::cout << "Document ID: " << docId << std::endl;
    if (allOccurrences.empty()) {
      std::cout << "  No occurrences found in this document." << std::endl;
    } else {
      std::cout << "  Occurrences at indices:" << std::endl;
      for (const auto &occurrence : allOccurrences) {
        std::cout << "    [";
        for (size_t i = 0; i < occurrence.size(); ++i) {
          std::cout << occurrence[i];
          if (i < occurrence.size() - 1) {
            std::cout << ", ";
          }
        }
        std::cout << "]" << std::endl;
      }
    }
    std::cout << "----------------------" << std::endl;
  }
  std::cout << "----------------------" << std::endl;
}
// TODO: Implement Missing words/tokens search
// TODO: Search ranking.
int searchNextToken(Posting &temp, std::string &nextToken, int lastToken) {

  auto tokenPosting = std::lower_bound(Index[nextToken].begin(),
                                       Index[nextToken].end(), temp, docIdComp);
  if (tokenPosting == Index[nextToken].end()) {
    return -1;
  }
  auto nextIdx = std::lower_bound(tokenPosting->positions.begin(),
                                  tokenPosting->positions.end(), lastToken + 1);
  if (nextIdx == tokenPosting->positions.end()) {
    return -1;
  }
  return *nextIdx;
  return -1;
}
void searchInDoc(int docId, std::vector<std::string> &tokensToSearch,
                 searchResults &serRes) {
  Posting temp;
  std::vector<const std::vector<int> *> PostingList;
  temp.docID = docId;
  for (int i = 0; i < tokensToSearch.size(); i++) {
    auto it = std::lower_bound(Index[tokensToSearch[i]].begin(),
                               Index[tokensToSearch[i]].end(), temp, docIdComp);
    if (it == Index[tokensToSearch[i]].end() || it->docID != docId) {
      return;
    }

    PostingList.push_back(&it->positions);
  }
  if (PostingList.empty())
    return;

  std::vector<int> currentMatch;

  for (int i = 0; i < PostingList[0]->size(); i++) {
    int lastIdx = (*PostingList[0])[i];
    currentMatch.push_back(lastIdx);
    for (int j = 1; j < PostingList.size(); j++) {
      auto tokenIt = std::lower_bound(PostingList[j]->begin(),
                                      PostingList[j]->end(), lastIdx + 1);
      if (tokenIt == PostingList[j]->end()) {
        return;
      }
      currentMatch.push_back(*tokenIt);
    }
    serRes[docId].push_back(currentMatch);
    currentMatch.clear();
  }
}
bool phraseSearch(std::vector<std::string> &tokensToSearch) {
  // for (int i = 0; i < tokensToSearch.size(); i++) {
  //   posts.push_back()
  // }
  std::vector<int> docScore(DocMap.size(), 0);
  for (auto token : tokensToSearch) {
    for (int i = 0; i < DocMap.size(); i++) {
      docScore[i] += TfIdf[{token, i}];
    }
  }
  std::vector<int> sortedDocs(DocMap.size());
  std::iota(sortedDocs.begin(), sortedDocs.end(), 0);

  std::sort(sortedDocs.begin(), sortedDocs.end(),
            [&docScore](int a, int b) { return docScore[a] > docScore[b]; });

  searchResults serRes;
  for (int i = 0; i < searchTopNDocs; i++) {
    searchInDoc(sortedDocs[i], tokensToSearch, serRes);
  }
  printSearchResults(serRes);
  return false;
}