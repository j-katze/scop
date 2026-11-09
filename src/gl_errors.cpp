#include "glad.h"
#include "gl_errors.hpp"

#include <iostream>

bool reportGlErrors(const char* where) {
    bool found = false;

    for (GLenum error; (error = glGetError()) != GL_NO_ERROR; ) {
        found = true;
        std::cerr << "OpenGL error 0x" << static_cast<unsigned>(error) << " at " << where << std::endl;
    }

    return found;
}
