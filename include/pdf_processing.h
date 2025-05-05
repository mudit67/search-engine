#ifndef PDF_PROCESSING_H
#define PDF_PROCESSING_H

#include <string>
#include <vector>
#include <filesystem>

namespace pdf_processing {
    // Extracts and returns text (per-page) from a single PDF file
    std::vector<std::string> extract_text_from_pdf(const std::filesystem::path& pdf_path);

    // Extracts all PDFs in a folder and returns a vector of vector<string> (all pages)
    std::vector<std::vector<std::string>> extract_texts_from_folder(const std::filesystem::path& folder_path);
}

#endif
