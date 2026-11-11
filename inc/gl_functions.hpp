#ifndef GL_FUNCTIONS_HPP
#define GL_FUNCTIONS_HPP

#include <GLFW/glfw3.h>
#include <GL/glcorearb.h>

class GLFunctions {
public:
    bool load();

    /* main */
    PFNGLVIEWPORTPROC                   Viewport                = nullptr;
    PFNGLENABLEPROC                     Enable                  = nullptr;
    PFNGLCLEARCOLORPROC                 ClearColor              = nullptr;
    PFNGLCLEARPROC                      Clear                   = nullptr;
    PFNGLPOLYGONMODEPROC                PolygonMode             = nullptr;

    /* shader */
    PFNGLCREATESHADERPROC               CreateShader            = nullptr;
    PFNGLSHADERSOURCEPROC               ShaderSource            = nullptr;
    PFNGLCOMPILESHADERPROC              CompileShader           = nullptr;
    PFNGLGETSHADERIVPROC                GetShaderiv             = nullptr;
    PFNGLGETSHADERINFOLOGPROC           GetShaderInfoLog        = nullptr;
    PFNGLCREATEPROGRAMPROC              CreateProgram           = nullptr;
    PFNGLATTACHSHADERPROC               AttachShader            = nullptr;
    PFNGLLINKPROGRAMPROC                LinkProgram             = nullptr;
    PFNGLGETPROGRAMIVPROC               GetProgramiv            = nullptr;
    PFNGLGETPROGRAMINFOLOGPROC          GetProgramInfoLog       = nullptr;
    PFNGLDELETESHADERPROC               DeleteShader            = nullptr;
    PFNGLDELETEPROGRAMPROC              DeleteProgram           = nullptr;
    PFNGLUSEPROGRAMPROC                 UseProgram              = nullptr;
    PFNGLGETUNIFORMLOCATIONPROC         GetUniformLocation      = nullptr;
    PFNGLUNIFORM1IPROC                  Uniform1i               = nullptr;
    PFNGLUNIFORM1FPROC                  Uniform1f               = nullptr;
    PFNGLUNIFORMMATRIX4FVPROC           UniformMatrix4fv        = nullptr;

    /* object */
    PFNGLACTIVETEXTUREPROC              ActiveTexture           = nullptr;
    PFNGLBINDTEXTUREPROC                BindTexture             = nullptr; // also used in bmp_image
    PFNGLBINDVERTEXARRAYPROC            BindVertexArray         = nullptr; // also used in obj_model
    PFNGLDRAWARRAYSPROC                 DrawArrays              = nullptr;

    /* obj_model */
    PFNGLDELETEBUFFERSPROC              DeleteBuffers           = nullptr;
    PFNGLDELETEVERTEXARRAYSPROC         DeleteVertexArrays      = nullptr;
    PFNGLGENVERTEXARRAYSPROC            GenVertexArrays         = nullptr;
    PFNGLGENBUFFERSPROC                 GenBuffers              = nullptr;
    PFNGLBINDBUFFERPROC                 BindBuffer              = nullptr;
    PFNGLBUFFERDATAPROC                 BufferData              = nullptr;
    PFNGLVERTEXATTRIBPOINTERPROC        VertexAttribPointer     = nullptr;
    PFNGLENABLEVERTEXATTRIBARRAYPROC    EnableVertexAttribArray = nullptr;

    /* bmp_image */
    PFNGLDELETETEXTURESPROC             DeleteTextures          = nullptr;
    PFNGLGENTEXTURESPROC                GenTextures             = nullptr;
    PFNGLTEXPARAMETERIPROC              TexParameteri           = nullptr;
    PFNGLPIXELSTOREIPROC                PixelStorei             = nullptr;
    PFNGLTEXIMAGE2DPROC                 TexImage2D              = nullptr;
    PFNGLGENERATEMIPMAPPROC             GenerateMipmap          = nullptr;

    /* gl_errors */
    PFNGLGETERRORPROC                   GetError                = nullptr;
};

extern GLFunctions gl;

#endif
