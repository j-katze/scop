#include "shader.hpp"
#include "gl_functions.hpp"

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
        vertex = gl.CreateShader(GL_VERTEX_SHADER);
        gl.ShaderSource(vertex, 1, &vShaderCode, NULL);
        gl.CompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        /* fragment shader */
        fragment = gl.CreateShader(GL_FRAGMENT_SHADER);
        gl.ShaderSource(fragment, 1, &fShaderCode, NULL);
        gl.CompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        /* shader program */
        ID_ = gl.CreateProgram();
        gl.AttachShader(ID_, vertex);
        gl.AttachShader(ID_, fragment);
        gl.LinkProgram(ID_);
        checkCompileErrors(ID_, "PROGRAM");

        /* delete the shaders as theyre no longer needed after linking */
        gl.DeleteShader(vertex);
        vertex = 0;
        gl.DeleteShader(fragment);
        fragment = 0;
    } catch (...) {
        if (ID_ != 0) {
            gl.DeleteProgram(ID_);
            ID_ = 0;
        }
        if (vertex != 0) {
            gl.DeleteShader(vertex);
        }
        if (fragment != 0) {
            gl.DeleteShader(fragment);
        }
        throw;
    }
}

Shader::~Shader() {
    destroy();
}

void Shader::use() {
    gl.UseProgram(ID_);
}

void Shader::destroy() {
    if (ID_ != 0) {
        gl.DeleteProgram(ID_);
        ID_ = 0;
    }
}

void Shader::setBool(const std::string &name, bool value) const {
    gl.Uniform1i(gl.GetUniformLocation(ID_, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
    gl.Uniform1i(gl.GetUniformLocation(ID_, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    gl.Uniform1f(gl.GetUniformLocation(ID_, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, const Mat4 &mat) const {
    gl.UniformMatrix4fv(gl.GetUniformLocation(ID_, name.c_str()), 1, GL_FALSE, mat.data());
}

void Shader::checkCompileErrors(unsigned int shader, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        gl.GetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            gl.GetShaderInfoLog(shader, 1024, NULL, infoLog);
            // std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog <<
            // "\n -- --------------------------------------------------- -- " << std::endl;
            throw std::runtime_error(
                    std::string("ERROR::SHADER_COMPILATION_ERROR of type: ") + type + "\n" + infoLog +
                    "\n -- --------------------------------------------------- -- "
            );
        }
    }
    else {
        gl.GetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            gl.GetProgramInfoLog(shader, 1024, NULL, infoLog);
            // std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog <<
            // "\n -- --------------------------------------------------- -- " << std::endl;
            throw std::runtime_error(
                    std::string("ERROR::PROGRAM_LINKING_ERROR of type: ") + type + "\n" + infoLog +
                    "\n -- --------------------------------------------------- -- "
            );
        }
    }
}
