#include "../include/pdf_processing.h"
#include <iostream>

int main() {
    const std::string folder = "assets/pdf_files";  // Replace with your actual folder path

    try {
        auto all_pdfs = pdf_processing::extract_texts_from_folder(folder);

        int pdf_index = 1;
        for (const auto& pdf : all_pdfs) {
            std::cout << "PDF #" << pdf_index++ << ":\n";
            int page = 1;
            for (const auto& text : pdf) {
                std::cout << "--- Page " << page++ << " ---\n";
                std::cout << text << "\n\n";
            }
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
