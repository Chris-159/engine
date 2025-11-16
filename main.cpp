#include <windows.h>
#include <vector>
#include <cstdint>
#include <cmath>
#include <iostream>

#include "math/mat4.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "math/camera.h"
#include "core/Model.h"
#include "renderer/Renderer.h"
#include "loader/ModelLoader.h"
#include "loader/ImageLoader.h"

using namespace math;
using namespace core;
using namespace renderer;
using namespace misc;

// -----------------------------------------------------------------------------
const int WIDTH = 800;
const int HEIGHT = 600;

std::vector<uint32_t> framebuffer(WIDTH * HEIGHT);

// -----------------------------------------------------------------------------
core::Model CreateTestModel()
{
    core::Model model;

    loader::ModelLoader loader;
    model = loader.LoadObj("models/pyramid_uv.obj");

    std::cout << "Loaded vertices: " << model.VertexCount() << "\n";
    std::cout << "Loaded faces: " << model.FaceCount() << "\n";
    
    return model;
}

core::Model CreateTestModel1()
// ---- QUICK MODEL LOADER TEST ----
{
    loader::ModelLoader ml;

    std::cout << "=============================\n";
    std::cout << " QUICK OBJ PARSER TEST START\n";
    std::cout << "=============================\n";

    core::Model testModel = ml.LoadObj("models/test1.obj");

    std::cout << "\n[TEST] Vertex count = " << testModel.vertices.size() << "\n";
    std::cout << "[TEST] Texcoord count = " << testModel.texcoords.size() << "\n";
    std::cout << "[TEST] Face count = " << testModel.faces.size() << "\n";

    std::cout << "\n[TEST] Dump faces:\n";
    for (int i = 0; i < testModel.faces.size(); i++)
    {
        const auto& f = testModel.faces[i];
        std::cout << "  Face " << i << " material=" << f.materialName << "\n";
        std::cout << "    V: ";
        for (int vi : f.vertexIndices) std::cout << vi << " ";
        std::cout << "\n    UV: ";
        for (int ti : f.texcoordIndices) std::cout << ti << " ";
        std::cout << "\n";
    }

    std::cout << "\n[TEST] Dump materials:\n";
    for (auto& [name, mat] : testModel.materials)
    {
        std::cout << "  Material " << name << "\n";
        std::cout << "    Kd: " << mat.color.x << " " << mat.color.y << " " << mat.color.z << "\n";
    }

    std::cout << "===========================\n";
    std::cout << " QUICK OBJ PARSER TEST END\n";
    std::cout << "===========================\n\n";

    return core::Model();
}
// ---- END TEST ----

// -----------------------------------------------------------------------------
// Renderer setup
Renderer renderer_(framebuffer.data(), WIDTH, HEIGHT);
core::Model model = CreateTestModel();
//core::Model m = CreateTestModel1();
bool loaded = false;
Camera cam;
Light sun;


// -----------------------------------------------------------------------------
void Render()
{
    renderer_.Clear(0x202020);
    static float camAngle = 0.0f;
    camAngle += 0.005f;

    // lighting
    sun.direction = Vec3(0.5f, -1.0f, -0.5f).Normalized(); // -0.5f, -0.3f, -1.0f
    sun.intensity = 1.2f;
    sun.ambient = 0.25f; // 0.1f
    renderer_.SetLight(sun);
    // ------

    float radius = 5.0f;
    float height = 1.5f + sinf(camAngle * 0.5f) * 0.5f;

    cam.position = Vec3(
        radius * sinf(camAngle),
        height,
        radius * cosf(camAngle)
    );
    cam.target = Vec3(0.0f, 0.5f, 0.0f);
    cam.up = Vec3(0, 1, 0);

    mat4 view = cam.GetViewMatrix();
    mat4 proj = cam.GetProjectionMatrix(0.5f, float(WIDTH) / HEIGHT, 0.1f, 20.0f);

    mat4 modelMat = mat4::Translation(math::Vec3(0, 0, 0)) *
                    mat4::RotationY(camAngle);

    renderer_.SetModelMatrix(modelMat);
    renderer_.SetCamera(view, proj);
    renderer_.SetActiveCamera(&cam);
    
    renderer_.DrawModel(model);
}

// -----------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        Sleep(16);
        Render();

        BITMAPINFO bmi = {0};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = WIDTH;
        bmi.bmiHeader.biHeight = -HEIGHT;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        StretchDIBits(hdc,
            0, 0, WIDTH, HEIGHT,
            0, 0, WIDTH, HEIGHT,
            framebuffer.data(), &bmi,
            DIB_RGB_COLORS, SRCCOPY);

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// -----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "RendererWindowClass";

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, "RendererWindowClass", "Software Renderer - Test Model",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        WIDTH, HEIGHT, nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            InvalidateRect(hwnd, nullptr, FALSE);
        }
    }

    return 0;
}
