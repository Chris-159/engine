#include <iostream>
#include <fstream>
#include <sstream>
#include "ModelLoader.h"

using namespace loader;

core::Model ModelLoader::LoadObj(const std::string& path) {
    core::Model model;
    std::ifstream file(path);
    std::string line;

    if (!file.is_open()) {
        std::cout << "[Loader] Failed to open file!\n";
        return model;
    }
    
    while (std::getline(file, line))
    {
        if (line.empty()) continue;

        if (line[0] == 'v' && line[1] == ' ') {
            ParseVertex(line, model);
        }
        
        if (line[0] == 'f' && line[1] == ' ') {
            ParseFace(line, model);
        }
    }

    file.close();
    return model;
}


/* PRIVATE */
bool ModelLoader::ParseVertex(const std::string& line, core::Model& model) {
    std::stringstream iss(line);
    char prefix;
    float x, y, z;

    iss >> prefix >> x >> y >> z;
    if (!iss.fail()) {
        model.vertices.emplace_back(x, y, z);
        return true;
    } else {
        std::cout << "[Loader-Vertex] Error at vertex line: " << line << "\n";
        return false;
    }
}

bool ModelLoader::ParseFace(const std::string& line, core::Model& model) {
    std::stringstream iss(line);
    char prefix;
    std::vector<int> indices;

    iss >> prefix;
    int index;
    while (iss >> index) {
        indices.push_back(index - 1); // using 0-based index
    }
    
    if (!indices.empty()) {
        model.faces.emplace_back(indices);
        return true;
    } else {
        std::cout << "[Loader-Face]: Faulty face line: " << line << "\n";
        return false;
    }
}