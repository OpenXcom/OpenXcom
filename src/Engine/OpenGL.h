// This file was copied from the bsnes project. 

// This is the license info, from ruby.hpp:

/*
  ruby
  version: 0.08 (2011-11-25)
  license: public domain
 */
#pragma once

#ifndef __NO_OPENGL

#include <SDL.h>
#include <SDL_opengl.h>
#include <string>

namespace OpenXcom
{

class Surface;

#ifndef __APPLE__
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLISPROGRAMPROC glIsProgram;
extern PFNGLISSHADERPROC glIsShader;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLDETACHSHADERPROC glDetachShader;
extern PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORM1IPROC glUniform1i;
extern PFNGLUNIFORM2FVPROC glUniform2fv;
extern PFNGLUNIFORM4FVPROC glUniform4fv;
#endif

std::string strGLError(GLenum glErr);

#define glErrorCheck() {\
	static bool reported = false;\
	GLenum glErr;\
	if (OpenGL::checkErrors && !reported && (glErr = glGetError()) != GL_NO_ERROR)\
	{\
		reported = true;\
		\
		do \
		{ \
			Log(LOG_WARNING) << __FILE__ << ":" << __LINE__ << ": glGetError() complaint: " << strGLError(glErr);\
		} while (((glErr = glGetError()) != GL_NO_ERROR));\
	}\
}


class OpenGL {
public:
  GLuint gltexture;
  GLuint glprogram;
  GLuint fragmentshader;
  bool linear;
  GLuint vertexshader;
  bool shader_support;

  uint32_t *buffer;
  Surface *buffer_surface;
  unsigned iwidth, iheight, iformat, ibpp;

  static bool checkErrors;

  /// call to resize internal buffer; internal use
  void resize(unsigned width, unsigned height);
  /// actually returns pointer to data buffer where one is to write the image
  bool lock(uint32_t *&data, unsigned &pitch);
  /// make all the pixels go away
  void clear();
  /// make the buffer show up on screen
  void refresh(bool smooth, unsigned inwidth, unsigned inheight, unsigned outwidth, unsigned outheight, int topBlackBand, int bottomBlackBand, int leftBlackBand, int rightBlackBand);
  /// set a shader! but what kind?
  void set_shader(const char *source);
  /// same but for fragment shader?
  void set_fragment_shader(const char *source);
  /// and vertex?
  void set_vertex_shader(const char *source);
  /// init(), because we're too cool to initialize everything in the constructor
  void init(int width, int height);
  /// more like exit, because destructors are for uncool people
  void term();
  /// Try to set VSync!
  void setVSync(bool sync);
  /// constructor -- like we said, we're too cool to actually construct things
  OpenGL();
  ~OpenGL();
};

}

#else

namespace OpenXcom { class OpenGL {}; }

#endif
