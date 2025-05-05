#include "../include/pdf_processing.h"
#include "../include/tokeniser.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace pdf_processing;
using namespace tokeniser;

int main() {
    string pdf_folder = "assets/pdf_files";
    string stopword_file = "assets/stopwords.txt";

    try {
        auto stopwords = load_stopwords(stopword_file);
        auto pdfs_text = extract_texts_from_folder(pdf_folder);

        int file_index = 1;
        for (const auto& pdf_pages : pdfs_text) {
            cout << "Tokenized Output for File #" << file_index++ << ":\n";
            auto token_stream = tokenize_pdf(pdf_pages, stopwords);

            for (size_t i = 0; i < token_stream.size(); ++i) {
                cout << " Page " << i + 1 << ": ";
                for (const auto& token : token_stream[i]) {
                    cout << token << " ";
                }
                cout << "\n";
            }
            cout << "--------------------------\n";
        }

    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << endl;
        return 1;
    }

    return 0;
}
