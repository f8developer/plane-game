#ifndef WINDOW_H
#define WINDOW_H

#include <string>


class Window {
public:
    Window(int width, int height, std::string title);
    ~Window();

    bool ShouldClose();

private:
    int width, height;
    std::string title;
};



#endif //WINDOW_H
