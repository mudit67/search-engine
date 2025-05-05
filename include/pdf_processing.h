#ifndef PDF_PROCESSING_H
#define PDF_PROCESSING_H

#include <string>
#include <filesystem>
#include <stdexcept>
#include <vector> // Include vector for returning page strings

namespace fs = std::filesystem;

namespace pdf_processing {
    // Modified function to return a vector of page texts
    std::vector<std::string> extract_text_from_pdf(const fs::path& pdf_path);
} // namespace pdf_processing

#endif
