// This file was copied from the bsnes project. 

// This is the license info, from ruby.hpp:

/*
  ruby
  version: 0.08 (2011-11-25)
  license: public domain
*/

#include <SDL.h>
#include <SDL_opengl.h>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <fstream>
#include <cstdlib>

#include "OpenGL.h"
#include "Logger.h"

namespace OpenXcom 
{
	
bool OpenGL::checkErrors = true;

std::string strGLError(GLenum glErr)
{
	std::string err;

	switch(glErr)
	{
	case GL_INVALID_ENUM:
		err = "GL_INVALID_ENUM";
		break;
	case GL_INVALID_VALUE:
		err = "GL_INVALID_VALUE";
		break;
	case GL_INVALID_OPERATION:
		err = "GL_INVALID_OPERATION";
		break;
	case GL_STACK_OVERFLOW:
		err = "GL_STACK_OVERFLOW";
		break;
	case GL_STACK_UNDERFLOW:
		err = "GL_STACK_UNDERFLOW";
		break;
	case GL_OUT_OF_MEMORY:
		err = "GL_OUT_OF_MEMORY";
		break;
	case GL_NO_ERROR:
		err = "No error! How did you even reach this code?";
		break;
	default:
		err = "Unknown error code!";
		break;
	}

	return err;
}



#define glGetProcAddress(name) SDL_GL_GetProcAddress(name)

#ifndef __APPLE__
PFNGLCREATEPROGRAMPROC glCreateProgram = 0;
PFNGLUSEPROGRAMPROC glUseProgram = 0;
PFNGLCREATESHADERPROC glCreateShader = 0;
PFNGLDELETESHADERPROC glDeleteShader = 0;
PFNGLSHADERSOURCEPROC glShaderSource = 0;
PFNGLCOMPILESHADERPROC glCompileShader = 0;
PFNGLATTACHSHADERPROC glAttachShader = 0;
PFNGLDETACHSHADERPROC glDetachShader = 0;
PFNGLLINKPROGRAMPROC glLinkProgram = 0;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = 0;
PFNGLUNIFORM1IPROC glUniform1i = 0;
PFNGLUNIFORM2FVPROC glUniform2fv = 0;
PFNGLUNIFORM4FVPROC glUniform4fv = 0;
#endif

void * (APIENTRYP glXGetCurrentDisplay)() = 0;
Uint32 (APIENTRYP glXGetCurrentDrawable)() = 0;
void (APIENTRYP glXSwapIntervalEXT)(void *display, Uint32 GLXDrawable, int interval);

Uint32 (APIENTRYP wglSwapIntervalEXT)(int interval);



  void OpenGL::resize(unsigned width, unsigned height) {
    if(gltexture == 0) glGenTextures(1, &gltexture);
	glErrorCheck();
	
    iwidth  = SDL_max(width,  iwidth );
    iheight = SDL_max(height, iheight);
    if(buffer_surface) delete buffer_surface;
    buffer_surface = new Surface(iwidth, iheight, 0, 0, ibpp); // use OpenXcom's Surface class to get an aligned buffer with bonus SDL_Surface
	buffer = (uint32_t*) buffer_surface->getSurface()->pixels;

    glBindTexture(GL_TEXTURE_2D, gltexture);
	glErrorCheck();
    glPixelStorei(GL_UNPACK_ROW_LENGTH, iwidth);
	glErrorCheck();
    glTexImage2D(GL_TEXTURE_2D,
      /* mip-map level = */ 0, /* internal format = */ GL_RGB16_EXT,
      width, height, /* border = */ 0, /* format = */ GL_BGRA,
      iformat, buffer);
	glErrorCheck();

  }

  bool OpenGL::lock(uint32_t *&data, unsigned &pitch) {
    pitch = iwidth * ibpp;
    return (data = buffer);
  }

  void OpenGL::clear() {
    memset(buffer, 0, iwidth * iheight * ibpp);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
  }

  void OpenGL::refresh(bool smooth, unsigned inwidth, unsigned inheight, unsigned outwidth, unsigned outheight) {
    while (glGetError() != GL_NO_ERROR); // clear possible error from who knows where
    if(shader_support && (fragmentshader || vertexshader)) {
      glUseProgram(glprogram);
      GLint location;

      float inputSize[2] = { (float)inwidth, (float)inheight };
      location = glGetUniformLocation(glprogram, "rubyInputSize");
      glUniform2fv(location, 1, inputSize);

      float outputSize[2] = { (float)outwidth, (float)outheight };
      location = glGetUniformLocation(glprogram, "rubyOutputSize");
      glUniform2fv(location, 1, outputSize);

      float textureSize[2] = { (float)iwidth, (float)iheight };
      location = glGetUniformLocation(glprogram, "rubyTextureSize");
      glUniform2fv(location, 1, textureSize);
    }

	glErrorCheck();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, smooth ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, smooth ? GL_LINEAR : GL_NEAREST);

	glErrorCheck();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, outwidth, 0, outheight, -1.0, 1.0);
    glViewport(0, 0, outwidth, outheight);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glErrorCheck();

    glPixelStorei(GL_UNPACK_ROW_LENGTH, buffer_surface->getSurface()->pitch / buffer_surface->getSurface()->format->BytesPerPixel);

	glErrorCheck();

    glTexSubImage2D(GL_TEXTURE_2D,
      /* mip-map level = */ 0, /* x = */ 0, /* y = */ 0,
      iwidth, iheight, GL_BGRA, iformat, buffer);


    //OpenGL projection sets 0,0 as *bottom-left* of screen.
    //therefore, below vertices flip image to support top-left source.
    //texture range = x1:0.0, y1:0.0, x2:1.0, y2:1.0
    //vertex range = x1:0, y1:0, x2:width, y2:height
    double w = double(inwidth)  / double(iwidth);
    double h = double(inheight) / double(iheight);
    int u = outwidth;
    int v = outheight;
	
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0, 0); glVertex3i(0, v, 0);
    glTexCoord2f(w, 0); glVertex3i(u, v, 0);
    glTexCoord2f(0, h); glVertex3i(0, 0, 0);
    glTexCoord2f(w, h); glVertex3i(u, 0, 0);
    glEnd();
	glErrorCheck();

    glFlush();
	glErrorCheck();

    if(shader_support) {
      glUseProgram(0);
    }
  }

  void OpenGL::set_shader(const char *source_yaml_filename) {
    if(!shader_support) return;

    if(fragmentshader) {
      glDetachShader(glprogram, fragmentshader);
      glDeleteShader(fragmentshader);
      fragmentshader = 0;
    }

    if(vertexshader) {
      glDetachShader(glprogram, vertexshader);
      glDeleteShader(vertexshader);
      vertexshader = 0;
    }

    if(source_yaml_filename && strlen(source_yaml_filename)) {
      std::ifstream fin(source_yaml_filename);
      YAML::Parser parser(fin);
      YAML::Node document;
	  parser.GetNextDocument(document);

      bool is_glsl;
	  std::string s;

	  document["language"] >> s;
	  is_glsl = (s == "GLSL");


      document["linear"] >> linear; // some shaders want texture linear interpolation and some don't
      std::string fragment_source;
      std::string vertex_source;
	  if (const YAML::Node *pFrag = document.FindValue("fragment")) *pFrag >> fragment_source;
	  if (const YAML::Node *pVert = document.FindValue("vertex")) *pVert >> vertex_source;

      if(is_glsl) {
        if(fragment_source != "") set_fragment_shader(fragment_source.c_str());
        if(vertex_source != "") set_vertex_shader(vertex_source.c_str());
      }
    }

    glLinkProgram(glprogram);
  }

  void OpenGL::set_fragment_shader(const char *source) {
    fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentshader, 1, &source, 0);
    glCompileShader(fragmentshader);
    glAttachShader(glprogram, fragmentshader);
  }

  void OpenGL::set_vertex_shader(const char *source) {
    vertexshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexshader, 1, &source, 0);
    glCompileShader(vertexshader);
    glAttachShader(glprogram, vertexshader);
  }

  void OpenGL::init(int w, int h) {
    //disable unused features
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_POLYGON_SMOOTH);
    glDisable(GL_STENCIL_TEST);

    //enable useful and required features
    glEnable(GL_DITHER);
    glEnable(GL_TEXTURE_2D);

    //bind shader functions
#ifndef __APPLE__
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)glGetProcAddress("glCreateProgram");
    glUseProgram = (PFNGLUSEPROGRAMPROC)glGetProcAddress("glUseProgram");
    glCreateShader = (PFNGLCREATESHADERPROC)glGetProcAddress("glCreateShader");
    glDeleteShader = (PFNGLDELETESHADERPROC)glGetProcAddress("glDeleteShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)glGetProcAddress("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)glGetProcAddress("glCompileShader");
    glAttachShader = (PFNGLATTACHSHADERPROC)glGetProcAddress("glAttachShader");
    glDetachShader = (PFNGLDETACHSHADERPROC)glGetProcAddress("glDetachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)glGetProcAddress("glLinkProgram");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glGetProcAddress("glGetUniformLocation");
    glUniform1i = (PFNGLUNIFORM1IPROC)glGetProcAddress("glUniform1i");
    glUniform2fv = (PFNGLUNIFORM2FVPROC)glGetProcAddress("glUniform2fv");
    glUniform4fv = (PFNGLUNIFORM4FVPROC)glGetProcAddress("glUniform4fv");
#endif
	glXGetCurrentDisplay = (void* (APIENTRYP)())glGetProcAddress("glXGetCurrentDisplay");
	glXGetCurrentDrawable = (Uint32 (APIENTRYP)())glGetProcAddress("glXGetCurrentDrawable");
	glXSwapIntervalEXT = (void (APIENTRYP)(void*, Uint32, int))glGetProcAddress("glXSwapIntervalEXT");

	wglSwapIntervalEXT = (Uint32 (APIENTRYP)(int))glGetProcAddress("wglSwapIntervalEXT");



    shader_support = glCreateProgram && glUseProgram && glCreateShader
    && glDeleteShader && glShaderSource && glCompileShader && glAttachShader
    && glDetachShader && glLinkProgram && glGetUniformLocation
    && glUniform1i && glUniform2fv && glUniform4fv;
	
    if(shader_support) glprogram = glCreateProgram();

    //create surface texture
    resize(w, h);
  }

	void OpenGL::setVSync(bool sync)
	{
		const int interval = sync ? 1 : 0;
		if (glXGetCurrentDisplay && glXGetCurrentDrawable && glXSwapIntervalEXT)
		{
			void *dpy = glXGetCurrentDisplay();
			Uint32 drawable = glXGetCurrentDrawable();

			if (drawable) {
				glXSwapIntervalEXT(dpy, drawable, interval);
				// Log(LOG_INFO) << "Made an attempt to set vsync via GLX.";
			}
		} else if (wglSwapIntervalEXT)
		{
			wglSwapIntervalEXT(interval);
			// Log(LOG_INFO) << "Made an attempt to set vsync via WGL.";
		}
	}

  void OpenGL::term() {
    if(gltexture) {
      glDeleteTextures(1, &gltexture);
      gltexture = 0;
    }

    if(buffer) {
      buffer = 0;
      iwidth = 0;
      iheight = 0;
    }

    delete buffer_surface;
  }

  OpenGL::OpenGL() : gltexture(0), glprogram(0), fragmentshader(0), linear(false), vertexshader(0),
                     buffer(NULL), buffer_surface(NULL), iwidth(0), iheight(0),
                     iformat(GL_UNSIGNED_INT_8_8_8_8_REV), // this didn't seem to be set anywhere before...
                     ibpp(32)                              // ...nor this
  { }

  OpenGL::~OpenGL() {
    term();
  }
}
