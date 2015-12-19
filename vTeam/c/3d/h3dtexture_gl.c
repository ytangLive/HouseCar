//
//  h3dtexture.c
//  hc3d
//
//  Created by Zhang Hailong on 12-11-26.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "h3dtexture.h"
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

#define GL_BGRA	GL_BGRA_EXT

#endif
c3d_texture_t c3d_texture_create(hany pixels ,TexturePixelFormat format,hint32 width,hint32 height){
   
    c3d_texture_t t = {0,width,height,0.0,0};
    
    glGenTextures(1, &t.texture);
	glBindTexture(GL_TEXTURE_2D, t.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format == TexturePixelFormatRGBA ? GL_RGBA : GL_BGRA, GL_UNSIGNED_BYTE, pixels);

    return t;
}

c3d_texture_t c3d_texture_bind(c3d_texture_t texture,hany pixels,TexturePixelFormat format,hint32 width,hint32 height){
    
    texture.width = width;
    texture.height = height;
    
    glBindTexture(GL_TEXTURE_2D, texture.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format == TexturePixelFormatRGBA ? GL_RGBA : GL_BGRA, GL_UNSIGNED_BYTE, pixels);
    
    return texture;
}

void c3d_texture_delete(c3d_texture_t texture){
    glDeleteTextures(1, &texture.texture);
}

void c3d_texture_active(c3d_texture_t texture,huint32 textureId){
    glActiveTexture(GL_TEXTURE0 + textureId);
    glBindTexture(GL_TEXTURE_2D, texture.texture);
}


