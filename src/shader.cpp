#include "shader.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

Shader::Shader(const char* vertexPath, const char* fragmentPath) : ID_(0) {
    std::string   vertexCode;
    std::string   fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    /* make sure it can throw exceptions */
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try {
        /* open files */
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        /* read files contents into streams */
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        /* close file handlers */
        vShaderFile.close();
        fShaderFile.close();
        /* convert stream into string */
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure&) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        throw;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    /* compile shaders */
    GLuint vertex = 0;
    GLuint fragment = 0;

    try {
        /* vertex shader */
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        /* fragment shader */
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        /* shader program */
        ID_ = glCreateProgram();
        glAttachShader(ID_, vertex);
        glAttachShader(ID_, fragment);
        glLinkProgram(ID_);
        checkCompileErrors(ID_, "PROGRAM");

        /* delete the shaders as theyre no longer needed after linking */
        glDeleteShader(vertex);
        vertex = 0;
        glDeleteShader(fragment);
        fragment = 0;
    } catch (...) {
        if (ID_ != 0) {
            glDeleteProgram(ID_);
            ID_ = 0;
        }
        if (vertex != 0) {
            glDeleteShader(vertex);
        }
        if (fragment != 0) {
            glDeleteShader(fragment);
        }
        throw;
    }
}

Shader::~Shader() {
    destroy();
}

void Shader::use() {
    glUseProgram(ID_);
}

void Shader::destroy() {
    if (ID_ != 0) {
        glDeleteProgram(ID_);
        ID_ = 0;
    }
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID_, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID_, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID_, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, const Mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID_, name.c_str()), 1, GL_FALSE, mat.data());
}

void Shader::checkCompileErrors(unsigned int shader, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            // std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog <<
            // "\n -- --------------------------------------------------- -- " << std::endl;
            throw std::runtime_error(
                    std::string("ERROR::SHADER_COMPILATION_ERROR of type: ") + type + "\n" + infoLog +
                    "\n -- --------------------------------------------------- -- "
            );
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            // std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog <<
            // "\n -- --------------------------------------------------- -- " << std::endl;
            throw std::runtime_error(
                    std::string("ERROR::PROGRAM_LINKING_ERROR of type: ") + type + "\n" + infoLog +
                    "\n -- --------------------------------------------------- -- "
            );
        }
    }
}
