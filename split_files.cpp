#include <windows.h>
#include <commdlg.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <direct.h>
#include <cstdlib>

void splitFile(const std::string& filePath, size_t chunkSizeMB, const std::string& outputFolder) {
    const size_t chunkSize = chunkSizeMB * 1024 * 1024;
    std::ifstream inputFile(filePath, std::ios::binary);
    
    if (!inputFile) {
        std::cerr << "Unable to open file for reading: " << filePath << std::endl;
        return;
    }

    if (_mkdir(outputFolder.c_str()) != 0 && errno != EEXIST) {
        std::cerr << "Unable to create directory: " << outputFolder << std::endl;
        return;
    }

    size_t chunkNumber = 0;
    std::vector<char> buffer(chunkSize);

    while (inputFile.read(buffer.data(), buffer.size()) || inputFile.gcount() > 0) {
        std::string chunkFilePath = outputFolder + "\\chunk_" + std::to_string(chunkNumber++) + ".bin";
        std::ofstream chunkFile(chunkFilePath, std::ios::binary);
        if (!chunkFile) {
            std::cerr << "Unable to create chunk file: " << chunkFilePath << std::endl;
            return;
        }
        chunkFile.write(buffer.data(), inputFile.gcount());
        if (!chunkFile) {
            std::cerr << "Error writing to chunk file: " << chunkFilePath << std::endl;
            return;
        }
    }

    if (!inputFile.eof()) {
        std::cerr << "Error during file read: " << filePath << std::endl;
    }
    
    std::cout << "File split into " << chunkNumber << " chunks and saved to folder: " << outputFolder << std::endl;
}


std::string openFileDialog() {
    OPENFILENAME ofn;
    char szFile[260]; 

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        return ofn.lpstrFile;
    }

    return "";
}

int main(int argc, char* argv[]) {

    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: splitfiles [chunk_size_MB]" << std::endl;
        return 1;
    }

    size_t chunkSizeMB = 24;
    if (argc == 3) {
        chunkSizeMB = std::atoi(argv[1]);
    }

    std::string filePath = openFileDialog();

    if (filePath.empty()) {
        std::cout << "No file selected." << std::endl;
        return 1;
    }

    std::string outputFolder = filePath.substr(0, filePath.find_last_of("\\/")) + "\\chunks";

    splitFile(filePath, chunkSizeMB, outputFolder);

    return 0;
}
