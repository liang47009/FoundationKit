/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_OPENGL_HPP
#define FOUNDATIONKIT_OPENGL_HPP

#pragma once
#include "FoundationKit/GenericPlatformMacros.hpp"

#if (PLATFORM_IOS)
    #define glClearDepth                glClearDepthf
    #define glDeleteVertexArrays        glDeleteVertexArraysOES
    #define glGenVertexArrays           glGenVertexArraysOES
    #define glBindVertexArray           glBindVertexArrayOES
    #define glMapBuffer                 glMapBufferOES
    #define glUnmapBuffer               glUnmapBufferOES
    #define GL_DEPTH24_STENCIL8         GL_DEPTH24_STENCIL8_OES
    #define GL_WRITE_ONLY               GL_WRITE_ONLY_OES
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>

#elif (PLATFORM_ANDROID)
    #define glClearDepth                glClearDepthf
    #define glDeleteVertexArrays        glDeleteVertexArraysOES
    #define glGenVertexArrays           glGenVertexArraysOES
    #define glBindVertexArray           glBindVertexArrayOES
    #define glMapBuffer                 glMapBufferOES
    #define glUnmapBuffer               glUnmapBufferOES
    #define GL_DEPTH24_STENCIL8         GL_DEPTH24_STENCIL8_OES
    #define GL_WRITE_ONLY               GL_WRITE_ONLY_OES
    // GL_GLEXT_PROTOTYPES isn't defined in glplatform.h on android ndk r7 
    // we manually define it here
    #include <GLES2/gl2platform.h>
    #ifndef GL_GLEXT_PROTOTYPES
    #define GL_GLEXT_PROTOTYPES 1
    #endif
    // normal process
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    // gl2.h doesn't define GLchar on Android
    typedef char GLchar;
    // android defines GL_BGRA_EXT but not GL_BRGA
    #ifndef GL_BGRA
    #define GL_BGRA  0x80E1
    #endif
    //declare here while define in EGLView_android.cpp
    extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOESEXT;
    extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOESEXT;
    extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOESEXT;
    #define glGenVertexArraysOES glGenVertexArraysOESEXT
    #define glBindVertexArrayOES glBindVertexArrayOESEXT
    #define glDeleteVertexArraysOES glDeleteVertexArraysOESEXT


#elif (PLATFORM_WINDOWS)
    #include <GL/GL.h>
    #define CC_GL_DEPTH24_STENCIL8      GL_DEPTH24_STENCIL8

#elif (PLATFORM_LINUX)
    #define CC_GL_DEPTH24_STENCIL8      GL_DEPTH24_STENCIL8

#elif (PLATFORM_MAC)
    #import <OpenGL/gl.h>
    #import <OpenGL/glu.h>
    #import <OpenGL/glext.h>
    #define CC_GL_DEPTH24_STENCIL8      -1
    #define glDeleteVertexArrays            glDeleteVertexArraysAPPLE
    #define glGenVertexArrays               glGenVertexArraysAPPLE
    #define glBindVertexArray               glBindVertexArrayAPPLE
    #define glClearDepthf                   glClearDepth
    #define glDepthRangef                   glDepthRange
    #define glReleaseShaderCompiler(xxx)

#endif
#endif // FOUNDATIONKIT_OPENGL_HPP
