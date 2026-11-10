#include "gl_functions.hpp"
#include <GL/glcorearb.h>

GLFunctions gl;

bool GLFunctions::load() {
    /* main */
    Viewport            = reinterpret_cast<PFNGLVIEWPORTPROC>(glfwGetProcAddress("glViewport"));
    Enable              = reinterpret_cast<PFNGLENABLEPROC>(glfwGetProcAddress("glEnable"));
    ClearColor          = reinterpret_cast<PFNGLCLEARCOLORPROC>(glfwGetProcAddress("glClearColor"));
    Clear               = reinterpret_cast<PFNGLCLEARPROC>(glfwGetProcAddress("glClear"));
    PolygonMode         = reinterpret_cast<PFNGLPOLYGONMODEPROC>(glfwGetProcAddress("glPolygonMode"));

    /* shader */
    CreateShader        = reinterpret_cast<PFNGLCREATESHADERPROC>(glfwGetProcAddress("glCreateShader"));
    ShaderSource        = reinterpret_cast<PFNGLSHADERSOURCEPROC>(glfwGetProcAddress("glShaderSource"));
    CompileShader       = reinterpret_cast<PFNGLCOMPILESHADERPROC>(glfwGetProcAddress("glCompileShader"));
    GetShaderiv         = reinterpret_cast<PFNGLGETSHADERIVPROC>(glfwGetProcAddress("glGetShaderiv"));
    GetShaderInfoLog    = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>(glfwGetProcAddress("glGetShaderInfoLog"));
    CreateProgram       = reinterpret_cast<PFNGLCREATEPROGRAMPROC>(glfwGetProcAddress("glCreateProgram"));
    AttachShader        = reinterpret_cast<PFNGLATTACHSHADERPROC>(glfwGetProcAddress("glAttachShader"));
    LinkProgram         = reinterpret_cast<PFNGLLINKPROGRAMPROC>(glfwGetProcAddress("glLinkProgram"));
    GetProgramiv        = reinterpret_cast<PFNGLGETPROGRAMIVPROC>(glfwGetProcAddress("glGetProgramiv"));
    GetProgramInfoLog   = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>(glfwGetProcAddress("glGetProgramInfoLog"));
    DeleteShader        = reinterpret_cast<PFNGLDELETESHADERPROC>(glfwGetProcAddress("glDeleteShader"));
    DeleteProgram       = reinterpret_cast<PFNGLDELETEPROGRAMPROC>(glfwGetProcAddress("glDeleteProgram"));
    UseProgram          = reinterpret_cast<PFNGLUSEPROGRAMPROC>(glfwGetProcAddress("glUseProgram"));
    GetUniformLocation  = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>(glfwGetProcAddress("glGetUniformLocation"));
    Uniform1i           = reinterpret_cast<PFNGLUNIFORM1IPROC>(glfwGetProcAddress("glUniform1i"));
    Uniform1f           = reinterpret_cast<PFNGLUNIFORM1FPROC>(glfwGetProcAddress("glUniform1f"));
    UniformMatrix4fv    = reinterpret_cast<PFNGLUNIFORMMATRIX4FVPROC>(glfwGetProcAddress("glUniformMatrix4fv"));

    /* object */
    ActiveTexture       = reinterpret_cast<PFNGLACTIVETEXTUREPROC>(glfwGetProcAddress("glActiveTexture"));
    BindTexture         = reinterpret_cast<PFNGLBINDTEXTUREPROC>(glfwGetProcAddress("glBindTexture"));
    BindVertexArray     = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(glfwGetProcAddress("glBindVertexArray"));
    DrawArrays          = reinterpret_cast<PFNGLDRAWARRAYSPROC>(glfwGetProcAddress("glDrawArrays"));

    /* obj_model */
    DeleteBuffers       = reinterpret_cast<PFNGLDELETEBUFFERSPROC>(glfwGetProcAddress("glDeleteBuffers"));
    DeleteVertexArrays  = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>(glfwGetProcAddress("glDeleteVertexArrays"));
    GenVertexArrays     = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(glfwGetProcAddress("glGenVertexArrays"));
    GenBuffers          = reinterpret_cast<PFNGLGENBUFFERSPROC>(glfwGetProcAddress("glGenBuffers"));
    BindBuffer          = reinterpret_cast<PFNGLBINDBUFFERPROC>(glfwGetProcAddress("glBindBuffer"));
    BufferData          = reinterpret_cast<PFNGLBUFFERDATAPROC>(glfwGetProcAddress("glBufferData"));
    VertexAttribPointer = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>(glfwGetProcAddress("glVertexAttribPointer"));
    EnableVertexAttribArray =
        reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(glfwGetProcAddress("glEnableVertexAttribArray"));

    /* bmp_image */
    DeleteTextures      = reinterpret_cast<PFNGLDELETETEXTURESPROC>(glfwGetProcAddress("glDeleteTextures"));
    GenTextures         = reinterpret_cast<PFNGLGENTEXTURESPROC>(glfwGetProcAddress("glGenTextures"));
    TexParameteri       = reinterpret_cast<PFNGLTEXPARAMETERIPROC>(glfwGetProcAddress("glTexParameteri"));
    PixelStorei         = reinterpret_cast<PFNGLPIXELSTOREIPROC>(glfwGetProcAddress("glPixelStorei"));
    TexImage2D          = reinterpret_cast<PFNGLTEXIMAGE2DPROC>(glfwGetProcAddress("glTexImage2D"));
    GenerateMipmap      = reinterpret_cast<PFNGLGENERATEMIPMAPPROC>(glfwGetProcAddress("glGenerateMipmap"));

    /* gl_errors */
    GetError            = reinterpret_cast<PFNGLGETERRORPROC>(glfwGetProcAddress("glGetError"));

    return Viewport                 && // main
           Enable                   &&
           ClearColor               &&
           Clear                    &&
           PolygonMode              &&
           CreateShader             && // shader
           ShaderSource             &&
           CompileShader            &&
           GetShaderiv              &&
           GetShaderInfoLog         &&
           CreateProgram            &&
           AttachShader             &&
           LinkProgram              &&
           GetProgramiv             &&
           GetProgramInfoLog        &&
           DeleteShader             &&
           DeleteProgram            &&
           UseProgram               &&
           GetUniformLocation       &&
           Uniform1i                &&
           Uniform1f                &&
           UniformMatrix4fv         &&
           ActiveTexture            && // object
           BindTexture              &&
           BindVertexArray          &&
           DrawArrays               &&
           DeleteBuffers            && // obj_model
           DeleteVertexArrays       &&
           GenVertexArrays          &&
           GenBuffers               &&
           BindBuffer               &&
           BufferData               &&
           VertexAttribPointer      &&
           EnableVertexAttribArray  &&
           DeleteTextures           && // bmp_image
           GenTextures              &&
           TexParameteri            &&
           PixelStorei              &&
           TexImage2D               &&
           GenerateMipmap           &&
           GetError;                   // gl_errors
}

