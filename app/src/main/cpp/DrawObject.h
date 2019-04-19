//
// Created by KIM Hyuntak on 15/04/2019.
//

#ifndef GLES3JNI_DRAWOBJECT_H
#define GLES3JNI_DRAWOBJECT_H

#include <cstring>
#include "glcommon.h"

struct DrawVtx {
  GLfloat coord[2];
  GLfloat pos[4];
};

struct DrawObject {

  float mPositionX; // centerX
  float mPositionY; // centerY
//  float mVectorX;
//  float mVectorY;
  float mWidth;
  float mHeight;

  float mCoordX;
  float mCoordY;
  float mCoordWidth;
  float mCoordHeight;
  float mTextureWidth;
  float mTextureHeight;
  float mTransform[4] = {0,0,0,0};

  void setTexcoordTransform(float* buf)
  {
    buf[0] = static_cast<float>(mCoordWidth) / mTextureWidth;
    buf[1] = 0;
    buf[2] = 0;
    buf[3] = static_cast<float>(mCoordHeight) / mTextureHeight;
  }

  void setTexcoordOffset(float* buf)
  {
    buf[0] = static_cast<float>(mCoordX) / mTextureWidth;
    buf[1] = static_cast<float>(mCoordY) / mTextureHeight;
  }

  void setPositionTransform(float* buf, int screenX, int screenY)
  {
    buf[0] = mWidth / screenX;
    buf[1] = 0;
    buf[2] = 0;
    buf[3] = mHeight / screenY;
  }

  void setPositionOffset(float* buf, int screenX, int screenY)
  {
    buf[0] = (2 * mPositionX - screenX) / (screenX);
    buf[1] = (2 * mPositionY - screenY) / (screenY);
  }

};

#endif //GLES3JNI_DRAWOBJECT_H
