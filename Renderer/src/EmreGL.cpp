#include "EmreGL.h"

// GLEW must be included before GLFW/OpenGL headers.
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace EmreGL {
    void PrintInfo()
    {
        std::cout << "EmreGL sandbox\n"
                  << "GLFW: " << glfwGetVersionString() << '\n'
                  << "GLEW: " << glewGetString(GLEW_VERSION) << '\n';
    }
}
