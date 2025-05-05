#include "../include/pdf_processing.h"
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
using namespace std;
namespace fs = std::filesystem;

// Function to process all PDF files in a folder
vector<vector<string>> extract_text_from_folder(const fs::path& folder_path) {
    vector<vector<string>> all_files_text;

    if (!fs::exists(folder_path) || !fs::is_directory(folder_path)) {
        cerr << "Error: Invalid folder path." << endl;
        return all_files_text; // Return empty vector
    }

    for (const auto& entry : fs::directory_iterator(folder_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".pdf") {
            try {
                // Call the function directly, no need for namespace qualification here
                vector<string> pages_text = pdf_processing::extract_text_from_pdf(entry.path());
                all_files_text.push_back(pages_text);
            } catch (const exception& e) {
                cerr << "Error processing file " << entry.path() << ": " << e.what() << endl;
            }
        }
    }
    return all_files_text;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <folder_path>" << endl;
        return 1;
    }

    fs::path folder_path(argv[1]);

    vector<vector<string>> extracted_text = extract_text_from_folder(folder_path);

    // Print the extracted text, file by file, page by page.
    for (size_t file_index = 0; file_index < extracted_text.size(); ++file_index) {
        cout << "File " << file_index + 1 << ":" << endl;
        for (size_t page_index = 0; page_index < extracted_text[file_index].size(); ++page_index) {
            cout << "  Page " << page_index + 1 << ":\n";
            cout << extracted_text[file_index][page_index] << endl;
        }
    }

    return 0;
}
