#include <poppler-document.h>
#include <poppler-page.h>
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <memory>
#include <vector>
#include <sstream>
#include <string>

using namespace std;
namespace fs = std::filesystem;

namespace pdf_processing {
    vector<string> extract_text_from_pdf(const fs::path& pdf_path) {
        cout << "Extracting text from PDF: " << pdf_path << endl;
        poppler::document* raw_doc = poppler::document::load_from_file(pdf_path.string());

        if (!raw_doc) {
            throw runtime_error("Could not open PDF file: " + pdf_path.string());
        }

        unique_ptr<poppler::document> document(raw_doc);

        vector<string> pages_text;
        for (int page_num = 0; page_num < document->pages(); ++page_num) {
            poppler::page* raw_page = document->create_page(page_num);
            if (raw_page) {
                unique_ptr<poppler::page> page(raw_page);
                string page_text(page->text().to_utf8().begin(), page->text().to_utf8().end());
                pages_text.push_back(page_text);
            }
        }
        return pages_text;
    }
}
