#ifndef __GLCOMMON_H__
#define __GLCOMMON_H__


#if DYNAMIC_ES3
#include "gl3stub.h"
#else
// Include the latest possible header file( GL version header )
#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif

#endif

struct Vertex {
  GLfloat pos[2];
  GLubyte rgba[4];
};

// returns true if a GL error occurred
bool checkGlError(const char* funcName);
void printGlString(const char* name, GLenum s);

#endif // __GLCOMMON_H__
