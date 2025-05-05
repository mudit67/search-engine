#include <algorithm>
#include <iostream>
#include <locale>
#include <memory> // For std::unique_ptr
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-global.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include <poppler/cpp/poppler-page.h>
#include <sstream>
#include <string>
#include <vector>

// std::stringstream tokenizeText(const std::string &pageText) {
//   std::stringstream tokenStream;
//   std::string currentToken;
//   std::locale loc;

//   for (char c : pageText) {
//     if (std::isspace(c, loc)) {
//       if (!currentToken.empty()) {
//         // Lowercase the token
//         std::transform(currentToken.begin(), currentToken.end(),
//                        currentToken.begin(),
//                        [](char ch) { return std::tolower(ch); });
//         tokenStream << currentToken << "\n";
//         currentToken.clear();
//       }
//     } else if (std::ispunct(c, loc)) {
//       // If we encounter punctuation and have a current token, emit it
//       if (!currentToken.empty()) {
//         std::transform(currentToken.begin(), currentToken.end(),
//                        currentToken.begin(),
//                        [](char ch) { return std::tolower(ch); });
//         tokenStream << currentToken << "\n";
//         currentToken.clear();
//       }
//       // Emit the punctuation as a separate token (optional, adjust as
//       needed)
//       // tokenStream << c << "\n";
//     } else {
//       currentToken += c;
//     }
//   }

//   // Emit the last token if any
//   if (!currentToken.empty()) {
//     std::transform(currentToken.begin(), currentToken.end(),
//                    currentToken.begin(),
//                    [](char ch) { return std::tolower(ch); });
//     tokenStream << currentToken << "\n";
//   }

//   return tokenStream;
//   // std::stringstream tokenStream;
//   // std::stringstream wordStream(pageText);
//   // std::string word;

//   // while (wordStream >> word) {
//   //   if (!word.empty()) {
//   //     std::string normalizedWord = word;
//   //     std::transform(
//   //         normalizedWord.begin(), normalizedWord.end(),
//   //         normalizedWord.begin(),
//   //         [](char c) { return std::tolower(c); }); // Using global locale
//   //     normalizedWord.erase(
//   //         std::remove_if(normalizedWord.begin(), normalizedWord.end(),
//   //                        [](unsigned char c) { return std::ispunct(c); }),
//   //         normalizedWord.end()); // Using global locale
//   //     tokenStream << normalizedWord << "\n";
//   //   }
//   // }
//   // return tokenStream;
// }

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
        currentToken +=
            c; // Basic attempt to handle "U.S.A." - needs more refinement
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

std::stringstream pdfTokenStream(const std::string &pdfPath) {
  std::stringstream overallTokenStream;
  std::unique_ptr<poppler::document> doc(
      poppler::document::load_from_file(pdfPath));

  if (!doc) {
    std::cerr << "Error: Could not open or load PDF file: " << pdfPath
              << std::endl;
    return overallTokenStream; // Return an empty stream on error
  }

  int num_pages = doc->pages();
  for (int i = 0; i < num_pages; ++i) {
    std::unique_ptr<poppler::page> page(doc->create_page(i));
    if (page) {
      std::string pageText = page->text().to_latin1();
      std::stringstream pageTokens = tokenizeText(pageText);
      overallTokenStream
          << pageTokens.rdbuf(); // Append all tokens from the page
    }
  }
  return overallTokenStream;
}

int main() {
  std::string pdf_path = "./bin/btee.pdf";
  std::unique_ptr<poppler::document> doc(
      poppler::document::load_from_file(pdf_path));

  if (doc) {
    int num_pages = doc->pages();
    for (int i = 0; i < num_pages; ++i) {
      std::unique_ptr<poppler::page> page(doc->create_page(i));
      if (page) {
        try {
          std::string text = page->text().to_latin1();
          std::cout << "Page " << i + 1 << " text extracted (" << text.length()
                    << " bytes)." << std::endl;
          // std::cout << text << std::endl;
          std::stringstream pageTokens = tokenizeText(text); // Comment out

          std::string token;

          while (pageTokens >> token) {
            std::cout << "Token: " << token << std::endl;
            // Process each token here without needing to store all of them
          }
          // tokenization
          // ...
        } catch (const std::bad_cast &e) {
          std::cerr << "Error on page " << i + 1 << ": " << e.what()
                    << std::endl;
          return 1; // Exit on error for now
        } catch (...) {
          std::cerr << "An unknown error occurred on page " << i + 1
                    << std::endl;
          return 1;
        }
      }
    }
  }
  return 0;
}
