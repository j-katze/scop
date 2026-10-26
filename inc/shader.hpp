#pragma once

#ifndef shader_hpp
#define shader_hpp

#include "../include/glad/glad.h"
#include "mat4.hpp"

#include <string>

class Shader {
public:
    /* program id */
    unsigned int ID;

    /* retreive source code */
    Shader(const char* vertexPath, const char* fragmentPath);

    /* activate the shader */
    void use();

    /* utility uniform functions */
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, const Mat4 &mat) const;

private:
    /* checking shader compile errors */
    void checkCompileErrors(unsigned int shader, std::string type);
};

#endif

