//
// Created by KIM Hyuntak on 10/04/2019.
//

#include "glcommon.h"
#include "Renderer.h"
#include "Shader.h"
#include "logger.h"
#include "image.h"
#include <EGL/egl.h>

#define STR(s) #s
#define STRV(s) STR(s)

//static const char VS[] =
//        "attribute vec4 position;\n"
////        "attribute vec2 texcoord;\n"
////        "varying vec2 texcoordVarying;\n"
////        "void main() {\n"
////        "gl_Position = position;\n"
////        "texcoordVarying = texcoord;\n"
////        "}\n";

#define POS_ATTRIB 0
#define SCALEROT_ATTRIB 1
#define OFFSET_ATTRIB 2
#define UVIN_ATTRIB 3

static const char VertextShader[] =
        "#version 300 es\n"
        "layout(location = 0) in vec3 position;\n"
        "layout(location = 1) in vec4 scalerot;\n"
        "layout(location = 2) in vec2 offset;\n"
        "layout(location = 3) in vec2 texcoord;\n"

        "out vec2 uv;\n"

        //"uniform mat4 MVP; \n"

        "void main() {\n"
        //"  mat2 sr = mat2(scalerot.xy, scalerot.zw); \n"
        //"  gl_Position = vec4(sr * position + offset, 0.0, 1.0); \n"
        "  gl_Position = vec4(position , 1.0);\n"
        "  uv = texcoord;\n"
        "}\n";


//const char* fragment_shader =
//        "precision mediump float;\n"
//        "varying vec2 texcoordVarying;\n"
//        "uniform sampler2D texture;\n"
//        "void main() {\n"
//        "gl_FragColor = texture2D(texture, texcoordVarying);\n"
//        "}\n";

static const char FragmentShader[] =
        "#version 300 es\n"

        "in vec2 uv;\n"
        "out vec4 color;\n"
        "uniform sampler2D textureSampler;\n"

        "void main() {\n"
        "  color = texture( textureSampler, uv ).rgba;\n"
        //"  color = vec4(1.0, 0.5, 0.5, 1.0);\n"
        "}\n";

static const float vertices[] = {
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f
};

static const float texcoords[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f
};

static const float scalerot[] = {
        100, 0 , 0 , 100,
        100, 0 , 0 , 100,
        100, 0 , 0 , 100,
        100, 0 , 0 , 100,
};
static const float offset[] = {
        0, 0,
        0, 0,
        0, 0,
        0, 0,
};


class TextureRenderer: public Renderer {
public:
  TextureRenderer();
  virtual ~TextureRenderer();
  bool init();

private:
  enum {VB_INISTANCE, VB_COUNT};

  virtual float* mapOffsetBuf();
  virtual void unmapOffsetBuf();
  virtual float* mapTransformBuf();
  virtual void unmapTransformBuf();
  virtual void draw(unsigned int numInstances);

  bool setTexture();

  const EGLContext mEglContext;
  GLuint mProgram;
  GLuint mVB[VB_COUNT];
  GLuint mVBState;
  GLuint mTexture;
  GLuint mTexcoord;
  GLuint mPosition;
  GLuint mScalerot;
  GLuint mOffset;

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

bool TextureRenderer::init()
{
  Shader shader{ VertextShader, FragmentShader };
  mProgram = shader.program();
  if (!mProgram) {
    ALOGE("Shader is not vallid.\n");
    return false;
  }

  glUseProgram(mProgram);
  mPosition = glGetAttribLocation(mProgram, "position");
  glEnableVertexAttribArray(mPosition);

  mTexcoord = glGetAttribLocation(mProgram, "texcoord");
  glEnableVertexAttribArray(mTexcoord);

  mScalerot = glGetAttribLocation(mProgram, "scalerot");
  glEnableVertexAttribArray(mScalerot);

  mOffset = glGetAttribLocation(mProgram, "offset");
  glEnableVertexAttribArray(mOffset);

  ALOGV("pos=%d, texcoord=%d, scalerot=%d, offset=%d", mPosition, mTexcoord, mScalerot, mOffset);

  if (!setTexture()) {
    ALOGE("Failed to initialize a texture.\n");
    return false;
  }

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

void TextureRenderer::draw(unsigned int numInstances)
{
  glUseProgram(mProgram);
  glBindTexture(GL_TEXTURE_2D, mTexture);
  checkGlError("glBindTexture");
  glVertexAttribPointer(mTexcoord, 2, GL_FLOAT, GL_FALSE, 0, texcoords);
  checkGlError("glVertexAttribPointer1");
  glVertexAttribPointer(mPosition, 3, GL_FLOAT, GL_FALSE, 0, vertices);
  checkGlError("glVertexAttribPointer2");
//  glVertexAttribPointer(mScalerot, 4, GL_FLOAT, GL_FALSE, 0, scalerot);
//  checkGlError("glVertexAttribPointer3");
//  glVertexAttribPointer(mOffset, 2, GL_FLOAT, GL_FALSE, 0, offset);
//  checkGlError("glVertexAttribPointer4");

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  checkGlError("glDrawArrays");


}