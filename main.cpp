#include<iostream>
#include "loader/ModelLoader.h"

using namespace std;

// [ model.obj ] → [ Loader ] → [ Model struktúra ] → [ Engine / Renderer ]

int main() {
    loader::ModelLoader loader;
    core::Model model = loader.LoadObj("test.obj");
    
    cout << "Vertexes: \n";
    for (auto & element : model.vertices) {
        cout << "X: " << element.x << " ";
        cout << "Y: " << element.y << " ";
        cout << "Z: " << element.z << " | ";
    }
    
    for (size_t i = 0; i < model.faces.size(); ++i) {
        std::cout << "Face " << i << ": ";
        for (int idx : model.faces[i].vertexIndices) {
            const auto& v = model.vertices[idx]; // VertexS
            std::cout << "(" << v.x << "," << v.y << "," << v.z << ") ";
        }
        std::cout << "\n";
    }

        
    cout << "\n\nVertex count: " << model.VertexCount();
    cout << "\nFace count: " << model.FaceCount();

    return 0;
}