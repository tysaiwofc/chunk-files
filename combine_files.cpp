#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

std::string getExecutableDirectory() {
    char exePath[MAX_PATH];
    if (GetModuleFileName(NULL, exePath, MAX_PATH) == 0) {
        std::cerr << "Failed to obtain the executable path." << std::endl;
        return "";
    }

    std::string exeDir = exePath;

    size_t pos = exeDir.find_last_of("\\/");
    if (pos != std::string::npos) {
        exeDir = exeDir.substr(0, pos + 1);
    } else {
        exeDir.clear();
    }

    return exeDir;
}

std::vector<std::string> listBinFiles(const std::string& directory) {
    std::vector<std::string> binFiles;
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((directory + "\\*.bin").c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to list files in the directory: " << directory << std::endl;
        return binFiles;
    }

    do {
        std::string filePath = directory + "\\" + findFileData.cFileName;
        binFiles.push_back(filePath);

        std::cout << "Found file: " << filePath << std::endl;
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
    return binFiles;
}

void combineBinFiles(const std::string& directory, const std::string& outputFile) {
    std::vector<std::string> binFiles = listBinFiles(directory);

    if (binFiles.empty()) {
        std::cerr << "No .bin files found in the directory." << std::endl;
        return;
    }

    std::ofstream output(outputFile, std::ios::binary);
    if (!output) {
        std::cerr << "Failed to create the output file." << std::endl;
        return;
    }

    for (const std::string& file : binFiles) {
        std::ifstream input(file, std::ios::binary);
        if (!input) {
            std::cerr << "Failed to open the file: " << file << std::endl;
            continue;
        }

        output << input.rdbuf();
    }

    std::cout << "Files combined into: " << outputFile << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: combinefiles <output_file_name>" << std::endl;
        return 1;
    }

    std::string currentDir = getExecutableDirectory();
    if (currentDir.empty()) {
        return 1;
    }

    std::string outputFile = argv[1];

    combineBinFiles(currentDir, outputFile);

    return 0;
}
