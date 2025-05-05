// #include "../bin/tokenDump.cpp"
#include "../include/dataStructures.h"
#include "../utils/pdftokenizer.cpp"
#include "./termRank.cpp"

extern InvertedIndex Index;
extern DocumentMapping DocMap;
extern termDocTable TfIdf;

// void printIndex(const InvertedIndex &index, const DocumentMapping
// &docMapping) {
//   std::cout << "--- Inverted Index ---" << std::endl;
//   for (const auto &pair : index) {
//     std::cout << "'" << pair.first << "': [";
//     bool first = true;
//     for (const auto &idFreqPair : pair.second) {
//       if (!first)
//         std::cout << ", ";
//       std::cout << idFreqPair.positions.size() << " : ";
//       for (int i = 0; i < idFreqPair.positions.size(); i++)
//         std::cout << idFreqPair.positions[i]
//                   << " "; // Optionally print the filename too:
//       if (idFreqPair.docID >= 0 && idFreqPair.docID < docMapping.size()) {
//         std::cout << " (" << docMapping[idFreqPair.docID]
//                   << ")"; // Print doc ID
//       }
//       first = false;
//     }
//     std::cout << "]" << std::endl;
//   }
//   std::cout << "----------------------" << std::endl;
//   std::cout << "\n--- Document Mapping ---" << std::endl;

//   for (int i = 0; i < docMapping.size(); ++i) {
//     std::cout << "ID " << i << ": " << docMapping[i] << std::endl;
//   }
//   std::cout << "------------------------" << std::endl;
// }
// std::vector<Posting>::iterator searchDocId(std::vector<Posting> &PostingArr,
//                                            int docId) {
//   auto it = std::lower_bound(
//       PostingArr.begin(), PostingArr.end(), docId,
//       [](const Posting &p, int targetDocId) { return p.docID < targetDocId;
//       });
//   return it;
// }

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
    int docId = DocMap.size();
    DocMap.push_back(pdfPath);

    for (int pageNum = 0; pageNum < doc->pages(); ++pageNum) {
      auto pageTokensWithPositions = getTokenInfosForPage(doc.get(), pageNum);
      for (size_t tokenIndex = 0; tokenIndex < pageTokensWithPositions.size();
           ++tokenIndex) {
        const auto &tokenInfo = pageTokensWithPositions[tokenIndex];
        if (!tokenInfo.text.empty()) {
          std::string tokenText = tokenInfo.text;
          poppler::rectf tokenBBox = tokenInfo.boundingBox;

          BoundingBox bbox;
          bbox.left = static_cast<int>(tokenBBox.left());
          bbox.top = static_cast<int>(tokenBBox.top());
          bbox.right = static_cast<int>(tokenBBox.right());
          bbox.bottom = static_cast<int>(tokenBBox.bottom());

          PositionInfo posInfo;
          posInfo.tokenIndex = static_cast<int>(tokenIndex);
          posInfo.bbox = bbox;
          posInfo.pageNumber = pageNum + 1; // Page numbers are usually 1-based

          auto it =
              std::lower_bound(Index[tokenText].begin(), Index[tokenText].end(),
                               docId, [](const Posting &p, int targetDocId) {
                                 return p.docID < targetDocId;
                               });

          if (it == Index[tokenText].end() || it->docID != docId) {
            Index[tokenText].insert(it, {docId, {posInfo}});
          } else {
            it->positions.push_back(posInfo);
            std::sort(it->positions.begin(), it->positions.end(),
                      [](const PositionInfo &a, const PositionInfo &b) {
                        return a.tokenIndex < b.tokenIndex;
                      });
          }
        }
      }
    }
  }
}

// void printIndex() {
//   std::cout << "--- Inverted Index ---" << std::endl;
//   for (const auto &pair : Index) {
//     std::cout << "Term: \"" << pair.first << "\"" << std::endl;
//     for (const auto &posting : pair.second) {
//       std::cout << "  Doc ID: " << posting.docID << std::endl;
//       std::cout << "    Positions: [";
//       for (const auto &posInfo : posting.positions) {
//         std::cout << "(" << posInfo.first << ", {" << posInfo.second.left
//                   << ", " << posInfo.second.top << ", " <<
//                   posInfo.second.right
//                   << ", " << posInfo.second.bottom << "}) ";
//       }
//       std::cout << "]" << std::endl;
//     }
//   }
//   std::cout << "--- End Inverted Index ---" << std::endl;
// }