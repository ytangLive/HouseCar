//
//  h3dbuffer_gl.c
//  hc3d
//
//  Created by Zhang Hailong on 13-3-31.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "h3dbuffer.h"
#include "hlog.h"



#ifdef __MACOS__

#ifdef __MACOSX__

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>

#else
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

#elif defined __ANDROID__

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#endif


c3d_buffer_t c3d_buffer_create(hint32 width,hint32 height){
    c3d_buffer_t rs = {0,0,width,height};

    glGenFramebuffers(1, &rs.frame);
    glBindFramebuffer(GL_FRAMEBUFFER, rs.frame);
    
    glGenTextures(1, &rs.texture);
    glBindTexture(GL_TEXTURE_2D, rs.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rs.texture, 0);


    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        hlog("Failure with framebuffer generation 0x%04x\n",glGetError());
    }
    
    glViewport(0, 0, width, height);
    
    glClearColor(1.0, 1.0, 1.0, 1.0);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    return rs;
}

void c3d_buffer_bind(c3d_buffer_t buffer){
    glBindFramebuffer(GL_FRAMEBUFFER, buffer.frame);
}

void c3d_buffer_active(c3d_buffer_t texture,huint32 textureId){
    glActiveTexture(GL_TEXTURE0 + textureId);
    glBindTexture(GL_TEXTURE_2D, texture.texture);
}


void c3d_buffer_delete(c3d_buffer_t buffer){
    glDeleteFramebuffers(1, &buffer.frame);
    glDeleteTextures(1,&buffer.texture);
}

