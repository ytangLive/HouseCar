//
//  h3dprogram.c
//  hc3d
//
//  Created by hailong zhang on 12-11-25.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "h3dprogram.h"
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


c3d_program_t c3d_program_create(hcchar * vshSource,hcchar * fshSource){
    
    c3d_program_t program = 0;
    GLint status = GL_TRUE;
    GLuint vshShader = 0,fshShader = 0;
    GLchar log[1024];
    GLint logLen = 0;
    
    vshShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshShader, 1, &vshSource, NULL);
    glCompileShader(vshShader);
    
    glGetShaderInfoLog(vshShader, sizeof(log), &logLen, log);
    
    if(logLen >0){
        hlog("Shader compile log:\n%s", log);
    }

    glGetShaderiv(vshShader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        
        glDeleteShader(vshShader);
        hlog("Failed to vsh compile shader\n");
        return program;
    }
    
    fshShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshShader, 1, &fshSource, NULL);
    glCompileShader(fshShader);
    
    
    glGetShaderInfoLog(fshShader, sizeof(log), &logLen, log);
    
    if(logLen >0){
        hlog("Shader compile log:\n%s", log);
    }
    
    glGetShaderiv(fshShader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        glDeleteShader(vshShader);
        glDeleteShader(fshShader);
        hlog("Failed to fsh compile shader");
        return program;
    }
    
    program = glCreateProgram();
    
    glAttachShader(program, vshShader);
    glAttachShader(program, fshShader);

    glLinkProgram(program);
    
    if (vshShader){
        glDeleteShader(vshShader);
    }
    
    if (fshShader){
        glDeleteShader(fshShader);
    }
    
    glGetProgramInfoLog(program, sizeof(log), &logLen, log);
    
    if(logLen >0){
        hlog("Program link log:\n%s", log);
    }
    
    status = GL_TRUE;
    
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    
    if (status != GL_TRUE ){
        
        hlog("Program link error");
        glDeleteProgram(program);
        program = 0;
        return program;
    }

    return program;
}

void c3d_program_delete(c3d_program_t program){
    glDeleteProgram(program);
}

void c3d_program_use(c3d_program_t program){
    glUseProgram(program);
}

c3d_location_t c3d_program_uniform_location(c3d_program_t program,hcchar * name){
    return glGetUniformLocation(program, name);
}

c3d_location_t c3d_program_attrib_location(c3d_program_t program,hcchar * name){
    return glGetAttribLocation(program, name);
}

void c3d_program_vertxt_attrib(c3d_location_t attrib,hint32 size,hany data,hint32 stride){
    glVertexAttribPointer(attrib, size, GL_FLOAT, GL_FALSE, stride, data);
    glEnableVertexAttribArray(attrib);
}

void c3d_program_uniform_1f(c3d_location_t uniform,float value){
    glUniform1f(uniform, value);
}

void c3d_program_uniform_1fv(c3d_location_t uniform,float * value,hint32 count){
    glUniform1fv(uniform, count, value);
}

void c3d_program_uniform_2f(c3d_location_t uniform,c3d_vector2_t value){
    glUniform2f(uniform, value.x, value.y);
}

void c3d_program_uniform_3f(c3d_location_t uniform,c3d_vector3_t value){
    glUniform3f(uniform, value.x, value.y, value.z);
}

void c3d_program_uniform_4f(c3d_location_t uniform,c3d_vector4_t value){
    glUniform4f(uniform, value.x, value.y, value.z, value.w);
}

void c3d_program_uniform_1i(c3d_location_t uniform,int value){
    glUniform1i(uniform, value);
}

void c3d_program_uniform_1iv(c3d_location_t uniform,int * value,hint32 count){
    glUniform1iv(uniform, count, value);
}

void c3d_program_uniform_4m(c3d_location_t uniform,c3d_matrix4_t value){
    glUniformMatrix4fv(uniform, 1, GL_FALSE, (float *)&value);
}

void c3d_program_uniform_3m(c3d_location_t uniform,c3d_matrix3_t value){
    glUniformMatrix3fv(uniform, 1, GL_FALSE, (float *)&value);
}

void c3d_program_uniform_3fv(c3d_location_t uniform,c3d_vector3_t * value ,hint32 count){
    glUniform3fv(uniform, count, (GLfloat *) value);
}

