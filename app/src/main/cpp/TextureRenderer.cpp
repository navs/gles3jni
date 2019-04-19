//
// Created by KIM Hyuntak on 10/04/2019.
//

#include "glcommon.h"
#include "Renderer.h"
#include "Shader.h"
#include "logger.h"
#include "image.h"
#include "DrawObject.h"
#include <EGL/egl.h>

#define POS_ATTRIB 0
#define TEX_ATTRIB 1
#define POS_TR_ATTRIB 2
#define TEX_TR_ATTRIB 3
#define POS_OFFSET_ATTRIB 4
#define TEX_OFFSET_ATTRIB 5

//#define STR(s) #s
//#define STRV(s) STR(s)

const float postex[16] = {
        -1, 1,  0, 0, // Left-Top
        1, 1, 1, 0, // Right-Top
        -1, -1, 0, 1, // Left-Bottom
        1, -1, 1, 1, // Right-Bottom;
};

static const char VertextShader[] =
        "#version 300 es\n"
        "layout(location = 0) in vec2 position;\n"
        "layout(location = 1) in vec2 texcoord;\n"

        "layout(location = 2) in vec4 pos_tr;\n"
        "layout(location = 3) in vec4 tex_tr;\n"
        "layout(location = 4) in vec2 pos_offset;\n"
        "layout(location = 5) in vec2 tex_offset;\n"

        "out vec2 uv;\n"

        "void main() {\n"
        "  mat2 px = mat2(pos_tr.x, pos_tr.y, pos_tr.z, pos_tr.w); \n"
        "  mat2 tx = mat2(tex_tr.x, tex_tr.y, tex_tr.z, tex_tr.w); \n"
        "  gl_Position = vec4(px * position + pos_offset , 0.0, 1.0); \n"
        "  uv = tx * texcoord + tex_offset;\n"
        "}\n";

static const char FragmentShader[] =
        "#version 300 es\n"

        "in vec2 uv;\n"
        "out vec4 color;\n"
        "uniform sampler2D textureSampler;\n"

        "void main() {\n"
        "  color = texture( textureSampler, uv ).rgba;\n"
        //"  color = vec4(1.0, 0.5, 0.5, 1.0);\n"
        "}\n";


class TextureRenderer: public Renderer {
public:
  TextureRenderer();
  virtual ~TextureRenderer();
  bool init();

private:
  enum {VB_INSTANCE, VB_POS_TR, VB_TEX_TR, VB_POS_OFFSET, VB_TEX_OFFSET, VB_COUNT};

  virtual float* mapOffsetBuf();
  virtual void unmapOffsetBuf();
  virtual float* mapTransformBuf();
  virtual void unmapTransformBuf();
  virtual void draw(unsigned int numInstances);
  virtual void onStep();

  bool setTexture();

  const EGLContext mEglContext;
  GLuint mProgram;
  GLuint mVB[VB_COUNT];
  GLuint mVBState;
  GLuint mTexture;

  DrawObject mDO[2];
};

Renderer* createTextureRenderer() {
  TextureRenderer* renderer = new TextureRenderer();
  if (!renderer->init()) {
    delete renderer;
    return NULL;
  }
  return renderer;
}

TextureRenderer::TextureRenderer()
        :   mEglContext(eglGetCurrentContext()),
            mProgram(0),
            mVBState(0)
{
  for (int i = 0; i < VB_COUNT; i++)
    mVB[i] = 0;
}

bool TextureRenderer::init() {
  Shader shader{VertextShader, FragmentShader};
  mProgram = shader.program();
  if (!mProgram) {
    ALOGE("Shader is not vallid.\n");
    return false;
  }

  //glUseProgram(mProgram);
  glGenBuffers(VB_COUNT, mVB);
  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_INSTANCE]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(postex), &postex[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_POS_TR]);
  glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_TEX_TR]);
  glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_POS_OFFSET]);
  glBufferData(GL_ARRAY_BUFFER, 2 * 2 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_TEX_OFFSET]);
  glBufferData(GL_ARRAY_BUFFER, 2 * 2 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

  glGenVertexArrays(1, &mVBState);
  glBindVertexArray(mVBState);

  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_INSTANCE]);
  glVertexAttribPointer(POS_ATTRIB, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
  glVertexAttribPointer(TEX_ATTRIB, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4,(const GLvoid *) (sizeof(float) * 2));
  glEnableVertexAttribArray(POS_ATTRIB);
  glEnableVertexAttribArray(TEX_ATTRIB);

  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_POS_TR]);
  glVertexAttribPointer(POS_TR_ATTRIB, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
  glEnableVertexAttribArray(POS_TR_ATTRIB);
  glVertexAttribDivisor(POS_TR_ATTRIB, 1);

  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_TEX_TR]);
  glVertexAttribPointer(TEX_TR_ATTRIB, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
  glEnableVertexAttribArray(TEX_TR_ATTRIB);
  glVertexAttribDivisor(TEX_TR_ATTRIB, 1);

  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_POS_OFFSET]);
  glVertexAttribPointer(POS_OFFSET_ATTRIB, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
  glEnableVertexAttribArray(POS_OFFSET_ATTRIB);
  glVertexAttribDivisor(POS_OFFSET_ATTRIB, 1);

  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_TEX_OFFSET]);
  glVertexAttribPointer(TEX_OFFSET_ATTRIB, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
  glEnableVertexAttribArray(TEX_OFFSET_ATTRIB);
  glVertexAttribDivisor(TEX_OFFSET_ATTRIB, 1);

  ALOGV("Loading Textures...\n");
  if (!setTexture()) {
    ALOGE("Failed to initialize a texture.\n");
    return false;
  }

  // init drawObject
  mDO[0].mWidth = 600;
  mDO[0].mHeight = 600;
  mDO[0].mPositionX = 500;
  mDO[0].mPositionY = 350;
  mDO[0].mCoordX = 1;
  mDO[0].mCoordY = 1;
  mDO[0].mCoordWidth = 16;
  mDO[0].mCoordHeight = 16;
  mDO[0].mTextureHeight = 128;
  mDO[0].mTextureWidth = 128;

  mDO[1].mWidth = 300;
  mDO[1].mHeight = 300;
  mDO[1].mPositionX = 150;
  mDO[1].mPositionY = 150;
  mDO[1].mCoordX = 35;
  mDO[1].mCoordY = 1;
  mDO[1].mCoordWidth = 16;
  mDO[1].mCoordHeight = 16;
  mDO[1].mTextureHeight = 128;
  mDO[1].mTextureWidth = 128;

  ALOGV("Using OpenGL ES 3.0 and TextureRenderer !\n");
  return true;
}

TextureRenderer::~TextureRenderer()
{
  if (eglGetCurrentContext() != mEglContext) {
    return;
  }

  // @TODO Finalize !
  glDeleteProgram(mProgram);
}

bool TextureRenderer::setTexture()
{

  if (!mProgram) {
    ALOGE("setTexture: The Shader (mProgram) is not valid.\n");
    return false;
  }


  mTexture = glGetUniformLocation(mProgram, "textureSampler");
  glGenTextures(1, &mTexture);
  glBindTexture(GL_TEXTURE_2D, mTexture);

  // Load PNG
  ALOGV("Loading image ... \n");
  PNG png("/sdcard/Download/sprite_01.png");
  ALOGV("Texture:%d, Image: alpha:%d width:%dpx height%dpx\n",
       mTexture,
       png.has_alpha(), png.get_width(), png.get_height());

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, png.has_alpha() ? GL_RGBA : GL_RGB,
               png.get_width(), png.get_height(),
               0, png.has_alpha() ? GL_RGBA : GL_RGB,
               GL_UNSIGNED_BYTE, png.get_data());
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  return true;
}

float* TextureRenderer::mapOffsetBuf()
{
//  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_OFFSET]);
//  return (float*)glMapBufferRange(GL_ARRAY_BUFFER,
//                                  0, MAX_INSTANCES * 2*sizeof(float),
//                                  GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
  return NULL;
}

void TextureRenderer::unmapOffsetBuf()
{
//  glUnmapBuffer(GL_ARRAY_BUFFER);
}

float* TextureRenderer::mapTransformBuf()
{
//  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_SCALEROT]);
//  return (float*)glMapBufferRange(GL_ARRAY_BUFFER,
//                                  0, MAX_INSTANCES * 4*sizeof(float),
//                                  GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
  return NULL;
}

void TextureRenderer::unmapTransformBuf()
{
//  glUnmapBuffer(GL_ARRAY_BUFFER);
}

void TextureRenderer::onStep()
{
  Renderer::onStep();

//  ALOGV("onStep::");

  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_POS_TR]);
  float* buf1 = (float*)glMapBufferRange(
          GL_ARRAY_BUFFER,
          0,
          2 * 4 * sizeof(float),
          GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
  mDO[0].setPositionTransform(&buf1[0], screenWidth(), screenHeight());
  mDO[1].setPositionTransform(&buf1[4], screenWidth(), screenHeight());
  glUnmapBuffer(GL_ARRAY_BUFFER);

  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_TEX_TR]);
  float* buf2 = (float*)glMapBufferRange(
          GL_ARRAY_BUFFER,
          0,
          2 * 4 * sizeof(float),
          GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
  mDO[0].setTexcoordTransform(&buf2[0]);
  mDO[1].setTexcoordTransform(&buf2[4]);
  glUnmapBuffer(GL_ARRAY_BUFFER);

  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_POS_OFFSET]);
  float* buf3 = (float*)glMapBufferRange(
          GL_ARRAY_BUFFER,
          0,
          2 * 2 * sizeof(float),
          GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
  mDO[0].setPositionOffset(&buf3[0], screenWidth(), screenHeight());
  mDO[1].setPositionOffset(&buf3[2], screenWidth(), screenHeight());
  glUnmapBuffer(GL_ARRAY_BUFFER);

  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_TEX_OFFSET]);
  float* buf4 = (float*)glMapBufferRange(
          GL_ARRAY_BUFFER,
          0,
          2 * 2 * sizeof(float),
          GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
  mDO[0].setTexcoordOffset(&buf4[0]);
  mDO[1].setTexcoordOffset(&buf4[2]);
  glUnmapBuffer(GL_ARRAY_BUFFER);
}

void TextureRenderer::draw(unsigned int numInstances)
{
  glUseProgram(mProgram);
  glBindTexture(GL_TEXTURE_2D, mTexture);
  checkGlError("glBindTexture");

  glEnable(GL_BLEND);
  glBindVertexArray(mVBState);
  checkGlError("glBindVertexArray");

  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 2);
  checkGlError("glDrawArraysInstanced");

  glDisable(GL_BLEND);


}