#include <cstring>
#include <filesystem> // Requires C++17
#include <iostream>
#include <string>
#include <vector>

#ifdef __linux__
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#endif

std::vector<std::string>
getAllPdfFiles(const std::string &directoryPath = "./bin") {
  std::cout << "Looking for pdf files" << std::endl;
  std::vector<std::string> pdfFilePaths;

#ifdef __linux__
  DIR *dir;
  struct dirent *ent;

  if ((dir = opendir(directoryPath.c_str())) != nullptr) {
    while ((ent = readdir(dir)) != nullptr) {
      std::string filename = ent->d_name;
      if (filename.length() > 4 &&
          filename.substr(filename.length() - 4) == ".pdf") {
        pdfFilePaths.push_back(directoryPath + "/" + filename);
      }
    }
    closedir(dir);
  } else {
    std::cerr << "Error opening directory: " << directoryPath << " ("
              << strerror(errno) << ")" << std::endl;
  }
#else
  std::cerr
      << "This function is currently only implemented for Linux-based systems."
      << std::endl;
#include <filesystem> // Requires C++17
#include <iostream>
#include <string>
#include <vector>

#ifdef __linux__
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#endif

  std::vector<std::string> getAllPdfFiles(const std::string &directoryPath =
                                              "/bin") {
    std::vector<std::string> pdfFilePaths;

#ifdef __linux__
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(directoryPath.c_str())) != nullptr) {
      while ((ent = readdir(dir)) != nullptr) {
        std::string filename = ent->d_name;
        if (filename.length() > 4 &&
            filename.substr(filename.length() - 4) == ".pdf") {
          pdfFilePaths.push_back(directoryPath + "/" + filename);
        }
      }
      closedir(dir);
    } else {
      std::cerr << "Error opening directory: " << directoryPath << " ("
                << strerror(errno) << ")" << std::endl;
    }
#else
    std::cerr << "This function is currently only implemented for Linux-based "
                 "systems."
              << std::endl;
#endif

    return pdfFilePaths;
  }
#endif

  return pdfFilePaths;
}