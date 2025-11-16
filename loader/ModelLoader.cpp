#include <iostream>
#include <fstream>
#include <sstream>

#include "ModelLoader.h"
#include "MaterialLoader.h"
#include "ImageLoader.h"

using namespace loader;

core::Model ModelLoader::LoadObj(const std::string& path_) {
    core::Model model;
    std::ifstream file(path_);
    std::string line;
    std::string currentMaterialName;

    if (!file.is_open()) {
        std::cout << "[Loader] Failed to open file!\n";
        return model;
    }
    
    while (std::getline(file, line))
    {
        if (line.empty()) continue;
        TrimString(line);
        if (line.empty()) continue;

        // find mtl file name
        if (line.rfind("mtllib", 0) == 0) {
            std::istringstream iss(line);
            std::string key, mtlFile;
            iss >> key >> mtlFile;

            std::string dir = path_.substr(0, path_.find_last_of("/\\") + 1);
            auto mats = MaterialLoader::LoadMTL(dir + mtlFile);

            model.materials.insert(mats.begin(), mats.end());
            std::cout << "[Loader] Loaded " << mats.size() << " materials from " << mtlFile << "\n";
        } else if (line.rfind("usemtl", 0) == 0) { // find which texture to use
            std::istringstream iss(line);
            std::string key;
            iss >> key >> currentMaterialName;

            std::cout << "[Loader] Using material: " << currentMaterialName << "\n";
        } else if (line.rfind("vt", 0) == 0) {
            ParseTexCoord(line, model);
        } else if (line[0] == 'v' && line[1] == ' ') {
            ParseVertex(line, model);
        } else if (line[0] == 'f' && line[1] == ' ') {
            ParseFace(line, currentMaterialName, model);
        }
    }

    std::cout << "[Loader] Materials loaded: " << model.materials.size() << "\n";
    for (const auto& [name, mat] : model.materials) {
        std::cout << "  Material: " << name << "\n"
                << "    Ka: " << mat.ambient.x << " " << mat.ambient.y << " " << mat.ambient.z << "\n"
                << "    Kd: " << mat.color.x << " " << mat.color.y << " " << mat.color.z << "\n"
                << "    Ks: " << mat.specular.x << " " << mat.specular.y << " " << mat.specular.z << "\n"
                << "    Ns: " << mat.shininess << "\n";
    }

    std::cout << "[Loader] Faces loaded: \n";
    for (int i = 0; i < model.faces.size(); i++) {
    std::cout << "Face " << i
              << " material: " << model.faces[i].materialName << "\n";
    }

    file.close();

    for (auto& [name, mat] : model.materials) {
        if (!mat.texturePath.empty()) {
            std::string dir = path_.substr(0, path_.find_last_of("/\\") + 1);
            mat.diffuseMap = loader::ImageLoader::Load(dir + mat.texturePath);
        }
    }

    return model;
}

void ModelLoader::TrimString(std::string &str_) {
    // simple trim (trim whitespace at start and beginning)
    size_t start = str_.find_first_not_of(" \t\r\n");

    if (start == std::string::npos) { str_.clear(); return; }
    
    size_t end = str_.find_last_not_of(" \t\r\n");
    str_ = str_.substr(start, end - start + 1);
}


/* PRIVATE */
bool ModelLoader::ParseVertex(const std::string& line_, core::Model& model_) {
    std::stringstream iss(line_);
    char prefix;
    float x, y, z;

    iss >> prefix >> x >> y >> z;
    if (!iss.fail()) {
        model_.vertices.emplace_back(x, y, z);
        return true;
    } else {
        std::cout << "[Loader-Vertex] Error at vertex line: " << line_ << "\n";
        return false;
    }
}

bool ModelLoader::ParseTexCoord(const std::string& line_, core::Model& model_) {
    // line format: "vt u v [w]"
    std::istringstream iss(line_);
    std::string prefix;
    float u = 0.0f, v = 0.0f;
    iss >> prefix >> u >> v;
    if (iss.fail()) {
        std::cout << "[Loader-UV] Error parsing texcoord line_: " << line_ << "\n";
        return false;
    }
    model_.texcoords.push_back({u, v});
    return true;
}

bool ModelLoader::ParseFace(const std::string& line_, const std::string& currentMaterialName_, core::Model& model_) {
    // Handles faces of form:
    // f v v v
    // f v/vt v/vt v/vt
    // f v/vt/vn v/vt/vn v/vt/vn
    // f v//vn v//vn v//vn
    std::istringstream iss(line_);
    char prefix;
    iss >> prefix; // 'f'

    std::vector<int> vIndices;
    std::vector<int> vtIndices;

    std::string token;
    while (iss >> token) {
        TrimString(token);
        if (token.empty()) continue;

        // split token by '/'
        int vi = -1, vti = -1, vni = -1;
        size_t p1 = token.find('/');
        if (p1 == std::string::npos) {
            // "v"
            vi = std::stoi(token) - 1;
        } else {
            // has at least one '/'
            std::string s1 = token.substr(0, p1);
            if (!s1.empty()) vi = std::stoi(s1) - 1;

            size_t p2 = token.find('/', p1 + 1);
            if (p2 == std::string::npos) {
                // "v/vt"
                std::string s2 = token.substr(p1 + 1);
                if (!s2.empty()) vti = std::stoi(s2) - 1;
            } else {
                // "v/vt/vn" or "v//vn"
                std::string s2 = token.substr(p1 + 1, p2 - (p1 + 1));
                if (!s2.empty()) vti = std::stoi(s2) - 1;

                std::string s3 = token.substr(p2 + 1);
                if (!s3.empty()) vni = std::stoi(s3) - 1;
            }
        }

        if (vi < 0) {
            std::cout << "[Loader-Face] Invalid vertex index in token: " << token << "\n";
            return false;
        }

        vIndices.push_back(vi);
        vtIndices.push_back(vti); // possibly -1
    }

    if (vIndices.size() < 3) {
        std::cout << "[Loader-Face] Face with less than 3 vertices: " << line_ << "\n";
        return false;
    }

    // Triangulate fan-style if polygon > 3 (v0,v(i),v(i+1))
    int v0 = vIndices[0];
    int vt0 = vtIndices[0];

    for (size_t i = 1; i + 1 < vIndices.size(); ++i) {
        core::Face f;

        f.vertexIndices.push_back(v0);
        f.vertexIndices.push_back(vIndices[i]);
        f.vertexIndices.push_back(vIndices[i+1]);

        // corresponding uv indices (may be -1)
        f.texcoordIndices.push_back(vt0);
        f.texcoordIndices.push_back(vtIndices[i]);
        f.texcoordIndices.push_back(vtIndices[i+1]);

        f.materialName = currentMaterialName_;
        model_.faces.push_back(std::move(f));
    }

    return true;
}


// bool ModelLoader::ParseFace(const std::string& line_, const std::string& materialName_, core::Model& model_) {
//     std::stringstream iss(line_);
//     char prefix;
//     std::vector<int> indices;

//     iss >> prefix;

//     int index;
//     while (iss >> index) {
//         indices.push_back(index - 1); // using 0-based index
//     }
    
//     if (!indices.empty()) {
//         model_.faces.emplace_back(indices, materialName_);
//         return true;
//     }

//     std::cout << "[Loader-Face]: Faulty face line: " << line_ << "\n";
//     return false;
// }