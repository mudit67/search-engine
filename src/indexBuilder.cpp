// #include "../bin/tokenDump.cpp"
#include "../include/dataStructures.h"
#include "../utils/pdftokenizer.cpp"
#include "./termRank.cpp"

extern InvertedIndex Index;
extern DocumentMapping DocMap;
extern termDocTable TfIdf;
void printIndex(const InvertedIndex &index, const DocumentMapping &docMapping) {
  std::cout << "--- Inverted Index ---" << std::endl;
  for (const auto &pair : index) {
    std::cout << "'" << pair.first << "': [";
    bool first = true;
    for (const auto &idFreqPair : pair.second) {
      if (!first)
        std::cout << ", ";
      std::cout << idFreqPair.positions.size() << " : ";
      for (int i = 0; i < idFreqPair.positions.size(); i++)
        std::cout << idFreqPair.positions[i]
                  << " "; // Optionally print the filename too:
      if (idFreqPair.docID >= 0 && idFreqPair.docID < docMapping.size()) {
        std::cout << " (" << docMapping[idFreqPair.docID]
                  << ")"; // Print doc ID
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
} // void buildIndex(DocumentMapping &DocMap, InvertedIndex &Index) {//  for
  // (int i = 0; i < processedDocuments.size(); i++) {//
  //  DocMap.push_back(processedDocuments[i].id);//   for (int j = 0; j <
  // processedDocuments[i].tokens.size(); j++) {//    std::string token =
  // processedDocuments[i].tokens[j];//    auto docIdIter =
  // searchDocId(Index[token], i);//    if (docIdIter == Index[token].end() ||//
  // (docIdIter != Index[token].end() && docIdIter->docID != i)) {//    //
  // Index[token].push_back({i, 1});//     insert_sorted(Index[token], {i,
  // std::vector<int>{j}}, docIdIter);//   } else {//    //
  // Index[token][docIdIter].frequency++;//
  //  docIdIter->positions.push_back(j);//   }//  }// }//  std::cout << "Index
  // Built!" << std::endl;//  printIndex(Index, DocMap);//  buildTfIDF();// //
  // return Index;// }
void processPdfAndBuildIndex(const std::string &pdfPath) {
  std::unique_ptr<poppler::document> doc = openPdfDocument(pdfPath);
  if (doc) {
    int num_pages = doc->pages();
    for (int i = 0; i < num_pages; ++i) {
      auto pageTokens = getTokenInfosForPage(doc.get(), i);
      if (pageTokens.size() > 0) {
        // for (auto token : pageTokens) {
        for (int i = 0; i < pageTokens.size(); i++) {
          std::cout << i << ":" << pageTokens.size() << std::endl;
          auto token = pageTokens[i];
          if (token.text.length() > 0) {
            std::cout << "check: ";
            std::cout << token.text << " " << token.boundingBox.top() << " "
                      << token.boundingBox.left() << " "
                      << token.boundingBox.bottom() << " "
                      << token.boundingBox.right() << std::endl;
            // indexBuilder.addToken(token, documentId,    i);

            // 'i' is the page number(0 - based)
            // }
          }
        }
      }
    }
  }
}