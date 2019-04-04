#ifndef __GLCOMMON_H__
#define __GLCOMMON_H__

#include <GLES3/gl3.h>

struct Vertex {
  GLfloat pos[2];
  GLubyte rgba[4];
};


// returns true if a GL error occurred
bool checkGlError(const char* funcName);
void printGlString(const char* name, GLenum s);

#endif // __GLCOMMON_H__
