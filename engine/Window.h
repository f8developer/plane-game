#ifndef WINDOW_H
#define WINDOW_H

#include <string>


class Window {
public:
    Window(int width, int height, std::string title);
    ~Window();

    bool ShouldClose();
    
    // Window size management
    void Resize(int newWidth, int newHeight);
    void ToggleFullscreen();
    bool IsFullscreen() const;
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

private:
    int width, height;
    std::string title;
    bool isFullscreen = false;
};



#endif //WINDOW_H
