#include "gl_functions.hpp"
#include <GL/glcorearb.h>
#include <GL/glx.h>

GLFunctions gl;

bool GLFunctions::load() {
    /* main */
    Viewport            = reinterpret_cast<PFNGLVIEWPORTPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glViewport")));
    Enable              = reinterpret_cast<PFNGLENABLEPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glEnable")));
    ClearColor          = reinterpret_cast<PFNGLCLEARCOLORPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glClearColor")));
    Clear               = reinterpret_cast<PFNGLCLEARPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glClear")));
    PolygonMode         = reinterpret_cast<PFNGLPOLYGONMODEPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glPolygonMode")));

    /* shader */
    CreateShader        = reinterpret_cast<PFNGLCREATESHADERPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glCreateShader")));
    ShaderSource        = reinterpret_cast<PFNGLSHADERSOURCEPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glShaderSource")));
    CompileShader       = reinterpret_cast<PFNGLCOMPILESHADERPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glCompileShader")));
    GetShaderiv         = reinterpret_cast<PFNGLGETSHADERIVPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glGetShaderiv")));
    GetShaderInfoLog    = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glGetShaderInfoLog")));
    CreateProgram       = reinterpret_cast<PFNGLCREATEPROGRAMPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glCreateProgram")));
    AttachShader        = reinterpret_cast<PFNGLATTACHSHADERPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glAttachShader")));
    LinkProgram         = reinterpret_cast<PFNGLLINKPROGRAMPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glLinkProgram")));
    GetProgramiv        = reinterpret_cast<PFNGLGETPROGRAMIVPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glGetProgramiv")));
    GetProgramInfoLog   = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glGetProgramInfoLog")));
    DeleteShader        = reinterpret_cast<PFNGLDELETESHADERPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glDeleteShader")));
    DeleteProgram       = reinterpret_cast<PFNGLDELETEPROGRAMPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glDeleteProgram")));
    UseProgram          = reinterpret_cast<PFNGLUSEPROGRAMPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glUseProgram")));
    GetUniformLocation  = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glGetUniformLocation")));
    Uniform1i           = reinterpret_cast<PFNGLUNIFORM1IPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glUniform1i")));
    Uniform1f           = reinterpret_cast<PFNGLUNIFORM1FPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glUniform1f")));
    UniformMatrix4fv    = reinterpret_cast<PFNGLUNIFORMMATRIX4FVPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glUniformMatrix4fv")));

    /* object */
    ActiveTexture       = reinterpret_cast<PFNGLACTIVETEXTUREPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glActiveTexture")));
    BindTexture         = reinterpret_cast<PFNGLBINDTEXTUREPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glBindTexture")));
    BindVertexArray     = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glBindVertexArray")));
    DrawArrays          = reinterpret_cast<PFNGLDRAWARRAYSPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glDrawArrays")));

    /* obj_model */
    DeleteBuffers       = reinterpret_cast<PFNGLDELETEBUFFERSPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glDeleteBuffers")));
    DeleteVertexArrays  = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glDeleteVertexArrays")));
    GenVertexArrays     = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glGenVertexArrays")));
    GenBuffers          = reinterpret_cast<PFNGLGENBUFFERSPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glGenBuffers")));
    BindBuffer          = reinterpret_cast<PFNGLBINDBUFFERPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glBindBuffer")));
    BufferData          = reinterpret_cast<PFNGLBUFFERDATAPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glBufferData")));
    VertexAttribPointer = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glVertexAttribPointer")));
    EnableVertexAttribArray =
        reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glEnableVertexAttribArray")));

    /* bmp_image */
    DeleteTextures      = reinterpret_cast<PFNGLDELETETEXTURESPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glDeleteTextures")));
    GenTextures         = reinterpret_cast<PFNGLGENTEXTURESPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glGenTextures")));
    TexParameteri       = reinterpret_cast<PFNGLTEXPARAMETERIPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glTexParameteri")));
    PixelStorei         = reinterpret_cast<PFNGLPIXELSTOREIPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glPixelStorei")));
    TexImage2D          = reinterpret_cast<PFNGLTEXIMAGE2DPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glTexImage2D")));
    GenerateMipmap      = reinterpret_cast<PFNGLGENERATEMIPMAPPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glGenerateMipmap")));

    /* gl_errors */
    GetError            = reinterpret_cast<PFNGLGETERRORPROC>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glGetError")));

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

