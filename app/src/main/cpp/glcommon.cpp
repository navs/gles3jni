#include "glcommon.h"
#include "logger.h"



void printGlString(const char* name, GLenum s)
{
  const char* v = (const char*)glGetString(s);
  ALOGV("GL %s: %s\n", name, v);
}

bool checkGlError(const char* funcName) {
  GLint err = glGetError();
  if (err != GL_NO_ERROR) {
    ALOGE("GL error after %s(): 0x%08x\n", funcName, err);
    return true;
  }
  return false;
}

