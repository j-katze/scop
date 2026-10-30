#ifndef SHADER_HPP
#define SHADER_HPP

#include "../include/glad/glad.h"
#include "mat4.hpp"

#include <string>

class Shader {
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    /* activate the shader */
    void use();

    void destroy();

    /* utility uniform functions */
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, const Mat4 &mat) const;

private:
    unsigned int ID_;

    /* checking shader compile errors */
    void checkCompileErrors(unsigned int shader, std::string type);
};

#endif
