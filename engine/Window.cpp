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

void Window::Resize(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    SetWindowSize(newWidth, newHeight);
}

void Window::ToggleFullscreen() {
    isFullscreen = !isFullscreen;
    if (isFullscreen) {
        int monitor = GetCurrentMonitor();
        int monitorWidth = GetMonitorWidth(monitor);
        int monitorHeight = GetMonitorHeight(monitor);
        SetWindowSize(monitorWidth, monitorHeight);
        SetWindowPosition(0, 0);
    } else {
        SetWindowSize(width, height);
        // Center the window on the current monitor
        int monitor = GetCurrentMonitor();
        int monitorWidth = GetMonitorWidth(monitor);
        int monitorHeight = GetMonitorHeight(monitor);
        int x = (monitorWidth - width) / 2;
        int y = (monitorHeight - height) / 2;
        SetWindowPosition(x, y);
    }
}

bool Window::IsFullscreen() const {
    return isFullscreen;
}


