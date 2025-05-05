#ifndef TOKENISER_H
#define TOKENISER_H

#include <string>
#include <vector>
#include <unordered_set>
#include <filesystem>

namespace tokeniser {
    // Loads stopwords from a file into an unordered_set
    std::unordered_set<std::string> load_stopwords(const std::filesystem::path& filepath);

    // Tokenizes a single page of text
    std::vector<std::string> tokenize(const std::string& text, const std::unordered_set<std::string>& stopwords);

    // Tokenizes a full PDF file (vector of pages)
    std::vector<std::vector<std::string>> tokenize_pdf(const std::vector<std::string>& pages, const std::unordered_set<std::string>& stopwords);
}

#endif
