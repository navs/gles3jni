#ifndef __SHADER_H__
#define __SHADER_H__

#include <GLES3/gl3.h>

class Shader {
  GLuint mProgram;
public:
  Shader(const char* vtxSrc, const char* fragSrc);

  GLuint program() const { return mProgram; }
};

#endif __SHADER_H__