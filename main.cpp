#include<iostream>

#include "loader/ModelLoader.h"
#include "transform/Transform.h"
#include "math/camera.h"

using namespace std;

// [ model.obj ] → [ Loader ] → [ Model struktúra ] → [ Engine / Renderer ]

int main() {
    math::Camera cam;
    cam.position = math::Vec3(0, 0, 0);
    cam.target   = math::Vec3(0, 0, -1);
    cam.up       = math::Vec3(0, 1, 0);

    math::mat4 view = cam.GetViewMatrix();

    std::cout << "View Matrix:\n";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << view.m[i][j] << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}