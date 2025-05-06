// #include "../bin/tokenDump.cpp"
#include "../include/dataStructures.h"
#include "../utils/pdftokenizer.cpp"
#include "./termRank.cpp"

extern InvertedIndex Index;
extern DocumentMapping DocMap;
extern termDocTable TfIdf;

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