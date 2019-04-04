#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <math.h>
#include "glcommon.h"

// ----------------------------------------------------------------------------
// Interface to the ES2 and ES3 renderers, used by JNI code.

extern const Vertex QUAD[4];


// ----------------------------------------------------------------------------
// Types, functions, and data used by both ES2 and ES3 renderers.
// Defined in gles3jni.cpp.

#define MAX_INSTANCES_PER_SIDE 16
#define MAX_INSTANCES   (MAX_INSTANCES_PER_SIDE * MAX_INSTANCES_PER_SIDE)
#define TWO_PI          (2.0 * M_PI)
#define MAX_ROT_SPEED   (0.3 * TWO_PI)

// This demo uses three coordinate spaces:
// - The model (a quad) is in a [-1 .. 1]^2 space
// - Scene space is either
//    landscape: [-1 .. 1] x [-1/(2*w/h) .. 1/(2*w/h)]
//    portrait:  [-1/(2*h/w) .. 1/(2*h/w)] x [-1 .. 1]
// - Clip space in OpenGL is [-1 .. 1]^2
//
// Conceptually, the quads are rotated in model space, then scaled (uniformly)
// and translated to place them in scene space. Scene space is then
// non-uniformly scaled to clip space. In practice the transforms are combined
// so vertices go directly from model to clip space.


class Renderer {
public:
  virtual ~Renderer();
  void resize(int w, int h);
  void render();

protected:
  Renderer();

  // return a pointer to a buffer of MAX_INSTANCES * sizeof(vec2).
  // the buffer is filled with per-instance offsets, then unmapped.
  virtual float* mapOffsetBuf() = 0;
  virtual void unmapOffsetBuf() = 0;
  // return a pointer to a buffer of MAX_INSTANCES * sizeof(vec4).
  // the buffer is filled with per-instance scale and rotation transforms.
  virtual float* mapTransformBuf() = 0;
  virtual void unmapTransformBuf() = 0;

  virtual void draw(unsigned int numInstances) = 0;

private:
  void calcSceneParams(unsigned int w, unsigned int h, float* offsets);
  void step();

  unsigned int mNumInstances;
  float mScale[2];
  float mAngularVelocity[MAX_INSTANCES];
  uint64_t mLastFrameNs;
  float mAngles[MAX_INSTANCES];
};

extern Renderer* createES2Renderer();
extern Renderer* createES3Renderer();

#endif // __RENDERER_H__