#include "../include/dataStructures.h"
#include <algorithm>
#include <iostream>
#include <locale>
#include <memory> // For std::unique_ptr
// #include <poppler/cpp/poppler-document.h>
// #include <poppler/cpp/poppler-global.h>
// #include <poppler/cpp/poppler-page-renderer.h>
// #include <poppler/cpp/poppler-page.h>
#include <sstream>
#include <string>
#include <vector>

std::stringstream tokenizeText(const std::string &pageText) {
  std::stringstream tokenStream;
  std::string currentToken;
  std::locale loc;

  for (size_t i = 0; i < pageText.length(); ++i) {
    char c = pageText[i];
    if (std::isspace(c, loc)) {
      if (!currentToken.empty()) {
        std::transform(currentToken.begin(), currentToken.end(),
                       currentToken.begin(), ::tolower);
        tokenStream << currentToken << "\n";
        currentToken.clear();
      }
    } else if (std::ispunct(c, loc)) {
      if (c == '\'' && i > 0 && i < pageText.length() - 1 &&
          std::isalpha(pageText[i - 1], loc) &&
          std::isalpha(pageText[i + 1], loc)) {
        currentToken += c; // Treat apostrophe in "don't" as part of the word
      } else if (c == '.' && i > 0 && std::isalnum(pageText[i - 1], loc) &&
                 i < pageText.length() - 1 &&
                 std::isalnum(pageText[i + 1], loc) &&
                 (i < pageText.length() - 2 &&
                  std::isupper(pageText[i + 1], loc) &&
                  pageText[i + 2] == '.')) {
        currentToken += c;
      } else if (!currentToken.empty()) {
        std::transform(currentToken.begin(), currentToken.end(),
                       currentToken.begin(), ::tolower);
        tokenStream << currentToken << "\n";
        currentToken.clear();
        // Optionally emit the punctuation as a separate token:
        // tokenStream << c << "\n";
      } else {
        // Optionally emit the punctuation as a separate token if no current
        // word tokenStream << c << "\n";
      }
    } else {
      currentToken += c;
    }
  }

  if (!currentToken.empty()) {
    std::transform(currentToken.begin(), currentToken.end(),
                   currentToken.begin(), ::tolower);
    tokenStream << currentToken << "\n";
  }

  return tokenStream;
}

// std::stringstream pdfTokenStream(const std::string &pdfPath) {
//   std::unique_ptr<poppler::document> doc(
//       poppler::document::load_from_file(pdfPath));
//   if (doc) {
//     int num_pages = doc->pages();
//     for (int i = 0; i < num_pages; ++i) {
//       std::unique_ptr<poppler::page> page(doc->create_page(i));
//       if (page) {
//         try {
//           std::string text = page->text().to_latin1();
//           std::cout << "Page " << i + 1 << " text extracted (" <<
//           text.length()
//                     << " bytes)." << std::endl;
//           std::stringstream pageTokens = tokenizeText(text); // Comment out
//         } catch (const std::bad_cast &e) {
//           std::cerr << "Error on page " << i + 1 << ": " << e.what()
//                     << std::endl;
//           return 1; // Exit on error for now
//         } catch (...) {
//           std::cerr << "An unknown error occurred on page " << i + 1
//                     << std::endl;
//           return 1;
//         }
//       }
//     }
//   }
// }

poppler::rectf unionRect(const poppler::rectf &r1, const poppler::rectf &r2) {
  if (r1.is_empty())
    return r2;
  if (r2.is_empty())
    return r1;
  double left = std::min(r1.left(), r2.left());
  double top = std::min(r1.top(), r2.top());
  double right = std::max(r1.right(), r2.right());
  double bottom = std::max(r1.bottom(), r2.bottom());
  return poppler::rectf(left, top, right - left, bottom - top);
}

std::vector<TokenInfo> getTokenInfosForPage(poppler::document *doc,
                                            int pageNumber) {
  std::vector<TokenInfo> tokensWithPositions;
  if (doc) {
    if (pageNumber >= 0 && pageNumber < doc->pages()) {
      std::unique_ptr<poppler::page> page(doc->create_page(pageNumber));
      if (page) {
        std::vector<poppler::text_box> text_list = page->text_list();
        std::locale loc;
        std::string currentToken;
        poppler::rectf currentBoundingBox;
        bool firstCharInToken = true;

        for (const auto &box : text_list) {
          std::string text = box.text().to_latin1(); // Or .to_utf8()
          for (char c : text) {
            if (std::isspace(c, loc)) {
              if (!currentToken.empty()) {
                std::transform(currentToken.begin(), currentToken.end(),
                               currentToken.begin(), ::tolower);

                tokensWithPositions.push_back(
                    {currentToken, currentBoundingBox});
                currentToken.clear();
                currentBoundingBox = poppler::rectf();
                firstCharInToken = true;
              }
            } else if (std::ispunct(c, loc)) {
              if (!currentToken.empty()) {
                std::transform(currentToken.begin(), currentToken.end(),
                               currentToken.begin(), ::tolower);
                tokensWithPositions.push_back(
                    {currentToken, currentBoundingBox});
                currentToken.clear();
                currentBoundingBox = poppler::rectf();
                firstCharInToken = true;
              }
              // Optionally treat punctuation as a separate token with its own
              // bbox tokensWithPositions.push_back({std::string(1, c),
              // box.bbox()});

            } else {
              currentToken += c;
              // Update bounding box to encompass all characters of the token
              if (firstCharInToken) {
                currentBoundingBox = box.bbox();
                firstCharInToken = false;
              } else {
                currentBoundingBox = unionRect(currentBoundingBox, box.bbox());
              }
            }
          }
          // Handle cases where a text_box ends a word
          if (!currentToken.empty() && !text.empty() &&
              !std::isspace(text.back(), loc)) {
            std::transform(currentToken.begin(), currentToken.end(),
                           currentToken.begin(), ::tolower);
            tokensWithPositions.push_back({currentToken, currentBoundingBox});
            currentToken.clear();
            currentBoundingBox = poppler::rectf();
            firstCharInToken = true;
          }
        }
      } else {
        std::cerr << "Error: Could not create page " << pageNumber + 1
                  << std::endl;
      }
    } else {
      std::cerr << "Error: Invalid page number: " << pageNumber + 1
                << std::endl;
    }
  } else {
    std::cerr << "Error: Document is not loaded." << std::endl;
  }
  return tokensWithPositions;
}

// Processes a single page and returns a stream of tokens for that page
std::stringstream getPageTokens(poppler::document *doc, int pageNumber) {
  std::stringstream pageTokenStream;
  if (doc) {
    if (pageNumber >= 0 && pageNumber < doc->pages()) {
      std::unique_ptr<poppler::page> page(doc->create_page(pageNumber));
      if (page) {
        std::string pageText = page->text().to_latin1();
        pageTokenStream << tokenizeText(pageText).rdbuf();
      } else {
        std::cerr << "Error: Could not create page " << pageNumber + 1
                  << std::endl;
      }
    } else {
      std::cerr << "Error: Invalid page number: " << pageNumber + 1
                << std::endl;
    }
  } else {
    std::cerr << "Error: Document is not loaded." << std::endl;
  }
  return pageTokenStream;
}

// Function to open the PDF document
std::unique_ptr<poppler::document> openPdfDocument(const std::string &pdfPath) {
  return std::unique_ptr<poppler::document>(
      poppler::document::load_from_file(pdfPath));
}