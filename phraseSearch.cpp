// #include <vector>
// #include <string>
#include "./indexBuilder.cpp"
#include <algorithm>
InvertedIndex Index;
DocumentMapping DocMap;
using searchResults = std::unordered_map<int, std::vector<std::vector<int>>>;

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
  temp.docID = docId;
  auto firstTokenPosting =
      std::lower_bound(Index[tokensToSearch[0]].begin(),
                       Index[tokensToSearch[0]].end(), temp, docIdComp);
  if (firstTokenPosting == Index[tokensToSearch[0]].end()) {
    std::cout << "First Token not found";
    return;
  }
  int tokenIdx = 0;
  for (int i = 0; i < firstTokenPosting->positions.size(); i++) {
    int lastIdx = firstTokenPosting->positions[i];
    std::vector<int> idxArray{lastIdx};
    for (int j = 1; j < tokensToSearch.size(); j++) {
      lastIdx = searchNextToken(temp, tokensToSearch[j], lastIdx);
      if (lastIdx == -1) {
        return;
      }
      idxArray.push_back(lastIdx);
    }
    serRes[docId].push_back(idxArray);
  }
}
bool phraseSearch(std::vector<std::string> &tokensToSearch) {
  buildIndex(DocMap, Index);
  // for (int i = 0; i < tokensToSearch.size(); i++) {
  //   posts.push_back()
  // }
  searchResults serRes;
  for (int i = 0; i < DocMap.size(); i++) {
    searchInDoc(i, tokensToSearch, serRes);
  }
  printSearchResults(serRes);
  return false;
}