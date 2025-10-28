#include<iostream>

#include "loader/ModelLoader.h"
#include "transform/Transform.h"
#include "math/camera.h"

using namespace std;

// [ model.obj ] → [ Loader ] → [ Model struktúra ] → [ Engine / Renderer ]

int main() {
    core::Model triangle;
    triangle.vertices.push_back(core::Vertex(0, 1, 0));
    triangle.vertices.push_back(core::Vertex(-1, -1, 0));
    triangle.vertices.push_back(core::Vertex(1, -1, 0));
    triangle.faces.push_back(core::Face({0, 1, 2}));

    math::Vec3 position(2, 0, -5);
    math::Vec3 rotation(0, M_PI/4, 0); // 45 fok Y tengely körül
    math::Vec3 scale(1, 1, 1);

    auto transformedVertices = transform::Transform::ApplyTRS(triangle, position, rotation, scale);


    math::Camera cam;
    cam.position = math::Vec3(0, 0, 0);
    cam.target   = math::Vec3(0, 0, -1);
    cam.up       = math::Vec3(0, 1, 0);

    math::mat4 viewMat = cam.GetViewMatrix();


    float fov    = 1.0f;   // egységnyi FOV (rad)
    float aspect = 1.0f;   // négyzetes képernyő
    float near   = 0.1f;
    float far    = 100.0f;

    math::mat4 projMat = cam.GetProjectionMatrix(fov, aspect, near, far);


    for (auto& v : transformedVertices) {
        math::Vec3 projected = cam.ProjectPoint(v, viewMat, projMat);
        std::cout << "Projected: " << projected.x << ", " << projected.y << ", " << projected.z << std::endl;
    }

    return 0;
}