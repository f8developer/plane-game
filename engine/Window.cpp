#include "Window.h"
#include "raylib.h"

Window::Window(int width, int height, std::string titleL):
    width {width},
    height {height},
    title {std::move(titleL)}
{
    InitWindow(width, height, title.c_str());
}

Window::~Window() {
    CloseWindow();
}

bool Window::ShouldClose() {
    return WindowShouldClose();
}


