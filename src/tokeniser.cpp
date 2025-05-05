#include "../include/tokeniser.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <cctype>
#include <algorithm>

using namespace std;

namespace tokeniser {

    unordered_set<string> load_stopwords(const filesystem::path& filepath) {
        unordered_set<string> stopwords;
        ifstream in(filepath);
        if (!in.is_open()) {
            throw runtime_error("Could not open stopwords file: " + filepath.string());
        }

        string word;
        while (getline(in, word)) {
            stopwords.insert(word);
        }

        return stopwords;
    }

    vector<string> tokenize(const string& text, const unordered_set<string>& stopwords) {
        vector<string> tokens;
        istringstream stream(text);
        string word;
        while (stream >> word) {
            // Remove punctuation
            word.erase(remove_if(word.begin(), word.end(), [](char ch) {
                return !isalpha(ch);
            }), word.end());

            transform(word.begin(), word.end(), word.begin(), ::tolower);

            if (!word.empty() && stopwords.find(word) == stopwords.end()) {
                tokens.push_back(word);
            }
        }
        return tokens;
    }

    vector<vector<string>> tokenize_pdf(const vector<string>& pages, const unordered_set<string>& stopwords) {
        vector<vector<string>> tokenized_pdf;
        for (const auto& page : pages) {
            tokenized_pdf.push_back(tokenize(page, stopwords));
        }
        return tokenized_pdf;
    }

}
