#include <windows.h>
#include <vector>
#include <cstdint>
#include <cmath>
#include <iostream>

#include "math/mat4.h"
#include "math/vec3.h"
#include "math/Vec4.h"
#include "math/Camera.h"
#include "transform/Transform.h"

using namespace math;
using namespace transform;

// -------------------------------------------------
// Global screen info
const int WIDTH = 800;
const int HEIGHT = 600;

std::vector<uint32_t> framebuffer(WIDTH * HEIGHT);

// -------------------------------------------------
void PutPixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
    framebuffer[y * WIDTH + x] = color;
}

// -------------------------------------------------
void Clear(uint32_t color = 0x000000) {
    std::fill(framebuffer.begin(), framebuffer.end(), color);
}

// -------------------------------------------------
void DrawLine(uint32_t* framebuffer, int width, int height,
              int x0, int y0, int x1, int y1, uint32_t color)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true)
    {
        if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height)
            framebuffer[y0 * width + x0] = color;

        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}


// -------------------------------------------------
void DrawTriangle(uint32_t* framebuffer, int width, int height,
                  const math::Vec3& v0, const math::Vec3& v1, const math::Vec3& v2, uint32_t color)
{
    auto toScreen = [&](const math::Vec3& v) -> std::pair<int,int> {
        int x = int((v.x * 0.5f + 0.5f) * width);
        int y = int((1.0f - (v.y * 0.5f + 0.5f)) * height);
        return {x, y};
    };

    auto [x0, y0] = toScreen(v0);
    auto [x1, y1] = toScreen(v1);
    auto [x2, y2] = toScreen(v2);

    DrawLine(framebuffer, width, height, x0, y0, x1, y1, color);
    DrawLine(framebuffer, width, height, x1, y1, x2, y2, color);
    DrawLine(framebuffer, width, height, x2, y2, x0, y0, color);
}

// -------------------------------------------------
math::Vec3 ProjectPoint(const math::Vec3& worldPos_, const math::mat4& view_, const math::mat4& projection_) {
    math::Vec4 p(worldPos_.x, worldPos_.y, worldPos_.z, 1.0f);
    math::Vec4 viewPos = view_ * p;
    math::Vec4 clipPos = projection_ * viewPos;

    if (fabs(clipPos.w) > 0.00001f) {
        clipPos.x /= clipPos.w;
        clipPos.y /= clipPos.w;
        clipPos.z /= clipPos.w;
    }

    return math::Vec3(clipPos.x, clipPos.y, clipPos.z);
}

// -------------------------------------------------
void Render() {
    Clear(0x101010);

    // Kamera és projekció setup
    Camera cam;
    cam.position = Vec3(0, 0, 3);
    cam.target = Vec3(0, 0, 0);
    cam.up = Vec3(0, 1, 0);

    mat4 view = cam.GetViewMatrix();
    mat4 proj = cam.GetProjectionMatrix(1.0f, float(WIDTH)/HEIGHT, 0.1f, 100.0f);

    // Teszt háromszög a világban
    Vec3 p1(-0.5f, -0.5f, 0.0f);
    Vec3 p2(0.5f, -0.5f, 0.0f);
    Vec3 p3(0.0f, 0.5f, 0.0f);

    // Projektálás
    Vec3 a = ProjectPoint(p1, view, proj);
    Vec3 b = ProjectPoint(p2, view, proj);
    Vec3 c = ProjectPoint(p3, view, proj);

    DrawTriangle(framebuffer.data(), WIDTH, HEIGHT, a, b, c, 0xFFFFFFFF);
}

// -------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            Render();

            BITMAPINFO bmi = {0};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = WIDTH;
            bmi.bmiHeader.biHeight = -HEIGHT; // flip
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

// -------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "3DWindowClass";

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, "3DWindowClass", "3D Projection Test",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        WIDTH, HEIGHT, nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            InvalidateRect(hwnd, nullptr, FALSE);
        }
    }

    return 0;
}
