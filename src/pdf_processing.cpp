#include "../include/pdf_processing.h"
#include <poppler-document.h>
#include <poppler-page.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <cctype>
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;

namespace pdf_processing
{

    // Helper function to clean and normalize text
    string clean_text(const string &raw_text)
    {
        stringstream cleaned;
        for (char ch : raw_text)
        {
            if (isalpha(ch) || isspace(ch))
            {
                cleaned << static_cast<char>(tolower(ch));
            }
            // skip punctuation and digits
        }

        // Normalize whitespace: collapse multiple spaces into one
        stringstream result;
        string word;
        while (cleaned >> word)
        {
            result << word << ' ';
        }

        return result.str();
    }

    vector<string> extract_text_from_pdf(const fs::path &pdf_path)
    {
        vector<string> page_texts;

        unique_ptr<poppler::document> doc(poppler::document::load_from_file(pdf_path.string()));
        if (!doc)
        {
            throw runtime_error("Failed to load PDF: " + pdf_path.string());
        }

        for (int i = 0; i < doc->pages(); ++i)
        {
            unique_ptr<poppler::page> page(doc->create_page(i));
            if (page)
            {
                auto byte_array = page->text().to_utf8();
                if (!byte_array.empty())
                {
                    string raw(byte_array.begin(), byte_array.end());
                    page_texts.push_back(clean_text(raw));
                }
                else
                {
                    cerr << "Warning: Empty page text on page " << i << " of " << pdf_path << endl;
                    page_texts.push_back("");
                }
            }
        }

        return page_texts;
    }

    vector<vector<string>> extract_texts_from_folder(const fs::path &folder_path)
    {
        vector<vector<string>> all_texts;

        if (!fs::exists(folder_path) || !fs::is_directory(folder_path))
        {
            throw runtime_error("Invalid folder path: " + folder_path.string());
        }

        for (const auto &entry : fs::directory_iterator(folder_path))
        {
            if (entry.path().extension() == ".pdf")
            {
                try
                {
                    auto pages = extract_text_from_pdf(entry.path());
                    all_texts.push_back(pages);
                }
                catch (const exception &e)
                {
                    cerr << "Error processing " << entry.path() << ": " << e.what() << endl;
                }
            }
        }

        return all_texts;
    }

}
