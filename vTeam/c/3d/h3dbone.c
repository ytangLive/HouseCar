//
//  h3dbone.c
//  hc3d
//
//  Created by zhang hailong on 12-12-3.
//  Copyright (c) 2012年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "h3dbone.h"
#include "hserial_list.h"
#include "hbuffer.h"
#include "hdata_alloc.h"
#include "hstr.h"
#include "hserial_map.h"

c3d_bone_offset_t c3d_bone_offset_identity(){
    c3d_bone_offset_t o;
    memset(&o,0,sizeof(c3d_bone_offset_t));
    o.transform = c3d_matrix3_identity;
    o.distance = 0.0;
    o.muscle = 0.0;
    o.muscleSwell = 0.0;
    return o;
}

void c3d_bone_context_cleanup(c3d_bone_context_t * context,InvokeTickDeclare){
    serial_list_dealloc(context->bones);
    serial_list_dealloc(context->positions);
    serial_list_dealloc(context->positions_anim);
    serial_list_dealloc(context->transforms);
    serial_list_dealloc(context->transforms_anim);
    serial_list_dealloc(context->distances);
    serial_list_dealloc(context->muscles);
    serial_list_dealloc(context->muscleSwells);
    hdata_dealloc(context->bonesMap);
    context->bones = NULL;
    context->positions = NULL;
    context->positions_anim = NULL;
    context->transforms = NULL;
    context->transforms_anim = NULL;
    context->distances = NULL;
    context->bonesMap = NULL;
    context->muscles = NULL;
    context->muscleSwells = NULL;
}

void c3d_bone_context_load(c3d_bone_context_t * context,hcchar * content,InvokeTickDeclare){
    
    hchar * p = (hchar *) content;
    hbuffer_t line = buffer_alloc(128, 128);
    hchar name[128];
    hfloat f;
    c3d_bone_t bone, * pbone = NULL;
    
    if(context->bones){
        serial_list_clear(context->bones);
    }
    else{
        context->bones = serial_list_alloc(sizeof(c3d_bone_t), 0);
    }
    if(context->positions){
        serial_list_clear(context->positions);
    }
    else{
        context->positions = serial_list_alloc(sizeof(c3d_vector3_t), 0);
    }
    if(context->positions_anim){
        serial_list_clear(context->positions_anim);
    }
    else{
        context->positions_anim = serial_list_alloc(sizeof(c3d_vector3_t), 0);
    }
    if(context->transforms){
        serial_list_clear(context->transforms);
    }
    else{
        context->transforms = serial_list_alloc(sizeof(c3d_matrix3_t), 0);
    }
    if(context->transforms_anim){
        serial_list_clear(context->transforms_anim);
    }
    else{
        context->transforms_anim = serial_list_alloc(sizeof(c3d_matrix3_t), 0);
    }
    if(context->distances){
        serial_list_clear(context->distances);
    }
    else{
        context->distances = serial_list_alloc(sizeof(float), 0);
    }
    
    if(context->muscles){
        serial_list_clear(context->muscles);
    }
    else{
        context->muscles = serial_list_alloc(sizeof(float), 0);
    }
    
    if(context->muscleSwells){
        serial_list_clear(context->muscleSwells);
    }
    else{
        context->muscleSwells = serial_list_alloc(sizeof(float), 0);
    }
    
    if(context->bonesMap){
        hdata_dealloc(context->bonesMap);
    }
    else{
        context->bonesMap = hdata_object_alloc();
    }
    
    while(p){
        
        if(*p == '\r'){
            
        }
        else if(*p == '\n' || *p =='\0'){
            if(buffer_length(line) && * buffer_data(line) != '#'){
                
                if(str_has_prefix(buffer_to_str(line), "-b ")){
                    
                    memset(&bone,0,sizeof(c3d_bone_t));
                    memset(name, 0, sizeof(name));
                    bone.transform = c3d_matrix3_identity;
                    bone.distance = 0.1;
                    bone.muscle = 0.5;
                    bone.size = 0.1;
                    bone.elbow = 0.0;
                    bone.minAffect = 0.0f;
                    bone.maxAffect = 1.0f;
                    sscanf(buffer_to_str(line), "-b %d %s"
                           ,&bone.parentIndex,name);
                    
                    if(bone.parentIndex < 0.0){
                        bone.parentIndex = serial_list_count(context->bones) + bone.parentIndex + 1;
                    }
                    
                    if(name[0] != '\0'){
                        hdata_object_put(context->bonesMap, name, hdata_int32_alloc(serial_list_count(context->bones)));
                    }
                    
                    pbone = (c3d_bone_t *)serial_list_item_add(context->bones, &bone);

                }
                else if(pbone && str_has_prefix(buffer_to_str(line), "s ") ){
                    sscanf(buffer_to_str(line), "s %f"
                           ,&pbone->size);
                }
                else if(pbone && str_has_prefix(buffer_to_str(line), "d ") ){
                    sscanf(buffer_to_str(line), "d %f"
                           ,&pbone->distance);
                }
                else if(pbone && str_has_prefix(buffer_to_str(line), "m ") ){
                    sscanf(buffer_to_str(line), "m %f"
                           ,&pbone->muscle);
                }
                else if(pbone && str_has_prefix(buffer_to_str(line), "e ") ){
                    sscanf(buffer_to_str(line), "e %f"
                           ,&pbone->elbow);
                }
                else if(pbone && str_has_prefix(buffer_to_str(line), "x ") ){
                    f = 0.0f;
                    sscanf(buffer_to_str(line), "x %f"
                           ,&f);
                    f = f * M_PI / 180.0;
                    pbone->transform = c3d_matrix3_rotate_x(pbone->transform, f);
                }
                else if(pbone && str_has_prefix(buffer_to_str(line), "y ") ){
                    f = 0.0f;
                    sscanf(buffer_to_str(line), "y %f"
                           ,&f);
                    f = f * M_PI / 180.0;
                    pbone->transform = c3d_matrix3_rotate_y(pbone->transform, f);
                }
                else if(pbone && str_has_prefix(buffer_to_str(line), "z ") ){
                    f = 0.0f;
                    sscanf(buffer_to_str(line), "z %f"
                           ,&f);
                    f = f * M_PI / 180.0;
                    pbone->transform = c3d_matrix3_rotate_z(pbone->transform, f);
                }
                else if(pbone && str_has_prefix(buffer_to_str(line), "min ") ){
                    sscanf(buffer_to_str(line), "min %f"
                           ,&pbone->minAffect);
                }
                else if(pbone && str_has_prefix(buffer_to_str(line), "max ") ){
                    sscanf(buffer_to_str(line), "max %f"
                           ,&pbone->maxAffect);
                }
                else if(pbone && str_has_prefix(buffer_to_str(line), "range ") ){
                    sscanf(buffer_to_str(line), "range %f %f %f %f %f %f"
                           ,&pbone->range.minVector.x,&pbone->range.minVector.y,&pbone->range.minVector.z
                           ,&pbone->range.maxVector.x,&pbone->range.maxVector.y,&pbone->range.maxVector.z);
                }
            }
            buffer_clear(line);
            if(*p == '\0'){
                break;
            }
        }
        else{
            buffer_append(line, p, 1);
        }
        
        p++;
    }
    

    
    buffer_dealloc(line);
    
}

void c3d_bone_context_load_file(c3d_bone_context_t * context,hcchar * filePath,InvokeTickDeclare){
    FILE * f = fopen(filePath, "r");
    hbuffer_t buf = buffer_alloc(MAX_READ_BUF, 128);
    hchar buffer[MAX_READ_BUF];
    hint32 len;
    if(f){
        
        while((len = fread(buffer, 1, sizeof(buffer), f)) >0){
            buffer_append(buf, buffer, len);
        }
        
        fclose(f);
        
        c3d_bone_context_load(context,buffer_to_str(buf),InvokeTickArg);
    }
    buffer_dealloc(buf);
}

void c3d_bone_context_anim(c3d_bone_context_t * context,c3d_bone_anim_t * anim,double current,InvokeTickDeclare){
    hint32 i,c = serial_list_count(context->bones);
    c3d_bone_t * bone ;
    c3d_vector3_t p1,p2,p;
    c3d_matrix3_t transform;
    c3d_bone_offset_t offset;
    hfloat distance,muscle,muscleSwell;
    
    serial_list_clear(context->positions_anim);
    serial_list_clear(context->transforms_anim);
    serial_list_clear(context->distances);
    serial_list_clear(context->muscles);
    serial_list_clear(context->muscleSwells);
  
    p = c3d_bone_anim_translate(anim,current,InvokeTickArg);
  
    p1 = * (c3d_vector3_t *) serial_list_item_at(context->positions, 0);
    
    p = c3d_vector3_add(p1, p);
    
    serial_list_item_add(context->positions_anim, &p);
    
    transform = c3d_matrix3_identity;
    
    serial_list_item_add(context->transforms_anim, &transform);
    
    
    distance = 0.0;
    
    serial_list_item_add(context->distances, &distance);
    
    muscle = 0.0;
    
    serial_list_item_add(context->muscles, &muscle);
    
    muscleSwell = 0.0;
    
    serial_list_item_add(context->muscleSwells, &muscleSwell);
    
    
    for(i=0;i<c;i++){
        bone = serial_list_item_at(context->bones, i);
        memset(&offset, 0, sizeof(c3d_bone_offset_t));
        offset.transform = c3d_matrix3_identity;
        
        if(anim){
            offset = c3d_bone_anim_offset(anim,current,i,InvokeTickArg);
        }
        
        assert(bone->parentIndex >=0 && bone->parentIndex <= c);
        
        distance = offset.distance  / bone->distance ;
        
        serial_list_item_add(context->distances, &offset.distance);
        
        muscle = offset.muscle;
        
        serial_list_item_add(context->muscles, &muscle);
        
        muscleSwell = offset.muscleSwell;
        
        serial_list_item_add(context->muscleSwells, &muscleSwell);
        
        p2 = * (c3d_vector3_t *) serial_list_item_at(context->positions, bone->parentIndex);
        p1 = * (c3d_vector3_t *) serial_list_item_at(context->positions, i +1);

        p =  c3d_vector3_multiply_scalar(c3d_vector3_sub(p1, p2), (1.0 + distance));

        transform = * (c3d_matrix3_t *) serial_list_item_at(context->transforms_anim, bone->parentIndex);
        
        transform = c3d_matrix3_multiply(transform,offset.transform);
        
        serial_list_item_add(context->transforms_anim, &transform);

        p = c3d_matrix3_multiply_vector3(transform, p);
        
        p2 = * (c3d_vector3_t *) serial_list_item_at(context->positions_anim, bone->parentIndex);
        
        p = c3d_vector3_add(p2, p);
        
        serial_list_item_add(context->positions_anim, &p);
        
    }
}

void c3d_bone_context_positions(c3d_bone_context_t * context,c3d_vector3_t position,InvokeTickDeclare){
    hint32 i,c = serial_list_count(context->bones);
    c3d_bone_t * bone;
    c3d_vector3_t p2,p;
    c3d_matrix3_t transform = c3d_matrix3_identity;
    hfloat distance;

    serial_list_clear(context->positions);
    serial_list_clear(context->transforms);
    serial_list_item_add(context->positions, &position);
    serial_list_item_add(context->transforms, &transform);
    
    for(i=0;i<c;i++){
        bone = serial_list_item_at(context->bones, i);
        
        assert(bone->parentIndex >=0 && bone->parentIndex <= c);
        
        distance = bone->distance  ;
        
        p2 = * (c3d_vector3_t *) serial_list_item_at(context->positions, bone->parentIndex);
        transform = * (c3d_matrix3_t *) serial_list_item_at(context->transforms, bone->parentIndex);
        
        p.x = 0.0f ;
        p.y = distance;
        p.z = 0.0f;
   
        transform = c3d_matrix3_multiply(bone->transform,transform);
        
        p = c3d_matrix3_multiply_vector3(transform, p);
        
        p = c3d_vector3_add(p2, p);
        
        serial_list_item_add(context->positions, &p);
        serial_list_item_add(context->transforms, &transform);
    }
}

int c3d_bone_context_index_for(c3d_bone_context_t * context,hcchar * name,InvokeTickDeclare){
    hdata_t v = hdata_object(&hdata_class, context->bonesMap, name);
    return hdata_int32(&hdata_class, v, -1);
}

c3d_vector3_t c3d_bone_anim_translate(c3d_bone_anim_t * anim,double current,InvokeTickDeclare){
    c3d_vector3_t v = {0.0f,0.0f,0.0f};
    hint32 i,c = serial_list_count(anim->frames);
    c3d_bone_anim_frame_t * frame ,*nframe;
    hdouble duration = 0.0;
    hfloat value;
    
    if(c > 1){
        for(i=0;i<c;i++){
            frame = (c3d_bone_anim_frame_t *) serial_list_item_at(anim->frames, i);
            duration += frame->duration;
            if(current <= duration){
                break;
            }
        }
        if(i + 1 < c ){
            if(frame->duration == 0.0){
                value = 1.0;
            }
            else{
                value = (current - duration + frame->duration) / frame->duration;
            }
            
            nframe = (c3d_bone_anim_frame_t *) serial_list_item_at(anim->frames, i + 1);
            
            v = c3d_vector3_anim(frame->translate,nframe->translate, value);
           
        }
        else{
            v = frame->translate;
        }
    }
    else if(c >0){
        frame = (c3d_bone_anim_frame_t *) serial_list_item_at(anim->frames, 0);
        v = frame->translate;
    }
    
    if(anim->parent.anim){
        v = c3d_vector3_add(c3d_bone_anim_translate(anim->parent.anim,anim->parent.duration,InvokeTickArg), v);
    }
    
    return v;

}

c3d_bone_offset_t c3d_bone_anim_offset(c3d_bone_anim_t * anim,double current,int index,InvokeTickDeclare){
    c3d_bone_offset_t v = {{0.0f},0.0f,0.0f}, * pfrom, * pto
        ,from = {{0.0f,0.0f,0.0f},0.0f,0.0f},to =  {{0.0f,0.0f,0.0f},0.0f,0.0f};
    hint32 i,c = serial_list_count(anim->frames);
    c3d_bone_anim_frame_t * frame ,*nframe;
    hdouble duration = 0.0;
    hfloat value;
    
    v.transform = c3d_matrix3_identity;
    from.transform = c3d_matrix3_identity;
    to.transform = c3d_matrix3_identity;
    
    if(c > 1){
        for(i=0;i<c;i++){
            frame = (c3d_bone_anim_frame_t *) serial_list_item_at(anim->frames, i);
            duration += frame->duration;
            if(current <= duration){
                break;
            }
        }
        if(i + 1 <c){
            if(frame->duration == 0.0){
                value = 1.0;
            }
            else{
                value = (current - duration + frame->duration) / frame->duration;
            }
            
            nframe = (c3d_bone_anim_frame_t *) serial_list_item_at(anim->frames, i + 1);
            pto = (c3d_bone_offset_t *) serial_map_get(nframe->offsets, &index);
            pfrom = (c3d_bone_offset_t *) serial_map_get(frame->offsets, &index);
            
            if(pfrom){
                from = * pfrom;
            }
            if(pto){
                to = * pto;
            }
            
            v = c3d_bone_offset_anim(from,to,value,InvokeTickArg);
        }
        else{
            pfrom = (c3d_bone_offset_t *) serial_map_get(frame->offsets, &index);
            if(pfrom){
                v = * pfrom;
            }
        }
    }
    else if(c >0){
        frame = (c3d_bone_anim_frame_t *) serial_list_item_at(anim->frames, 0);
        pfrom = (c3d_bone_offset_t *) serial_map_get(frame->offsets, &index);
        if(pfrom){
            v = * pfrom;
        }
    }
    
    if(anim->parent.anim){
        from = c3d_bone_anim_offset(anim->parent.anim,anim->parent.duration,index,InvokeTickArg);
        v.transform = c3d_matrix3_multiply(v.transform, from.transform);
        v.distance = v.distance + from.distance;
        v.muscle = v.muscle + from.muscle;
        v.muscleSwell = v.muscleSwell + from.muscleSwell;
    }
    
    return v;
}

void c3d_bone_anim_cleanup(c3d_bone_anim_t * anim,InvokeTickDeclare){
    hint32 i,c = serial_list_count(anim->frames);
    c3d_bone_anim_frame_t * frame;
    for(i=0;i<c;i++){
        frame = (c3d_bone_anim_frame_t *) serial_list_item_at(anim->frames, i);
        serial_map_dealloc(frame->offsets);
    }
    serial_list_dealloc(anim->frames);
    memset(anim, 0, sizeof(c3d_bone_anim_t));
}

void c3d_bone_anim_load(c3d_bone_context_t * context,c3d_bone_anim_t * anim,hcchar * content,InvokeTickDeclare){
    
    hchar * p = (hchar *) content;
    hbuffer_t line = buffer_alloc(128, 128);
    hchar name[128];
    c3d_bone_anim_frame_t * pframe = NULL,frame;
    c3d_bone_offset_t offset,*poffset = NULL;
    hint32 index;
    hint32 i,c;
    hfloat f;
    if(anim->frames){
        c = serial_list_count(anim->frames);
        for(i=0;i<c;i++){
            pframe = (c3d_bone_anim_frame_t *) serial_list_item_at(anim->frames, i);
            serial_map_dealloc(pframe->offsets);
        }
        serial_list_clear(anim->frames);
       
    }
    else{
        anim->frames = serial_list_alloc(sizeof(c3d_bone_anim_frame_t), 0);
    }
    
    anim->duration = 0.0;
    pframe = NULL;
    
    while( p){
        
        if(*p == '\r'){
            
        }
        else if(*p == '\n' || *p =='\0'){
            if(buffer_length(line) && * buffer_data(line) != '#'){
                
                if(str_has_prefix(buffer_to_str(line), "-key ")){
                    memset(&frame, 0, sizeof(frame));
                    sscanf(buffer_to_str(line), "-key %lf %f %f %f",&frame.duration,&frame.translate.x,&frame.translate.y,&frame.translate.z);
                    serial_list_item_add(anim->frames, &frame);
                    pframe = serial_list_item_last(anim->frames);
                    anim->duration += frame.duration;
                }
                else if(pframe && str_has_prefix(buffer_to_str(line), "-b ")){
                    
                    memset(name, 0, sizeof(name));
                    memset(&offset, 0, sizeof(c3d_bone_offset_t));
                    offset.transform = c3d_matrix3_identity;
                    sscanf(buffer_to_str(line), "-b %s"
                           ,name);
                    index = c3d_bone_context_index_for(context, name, InvokeTickArg);
                    if(index >=0){
                        if(pframe->offsets == NULL){
                            pframe->offsets = serial_map_alloc(sizeof(hint32), sizeof(c3d_bone_offset_t), 0);
                        }
                        poffset = (c3d_bone_offset_t *)serial_map_put(pframe->offsets, &index, &offset);
                    }
                    else{
                        poffset = NULL;
                    }
                }
                else if(poffset && str_has_prefix(buffer_to_str(line), "d ")){
                    sscanf(buffer_to_str(line), "d %f"
                           ,&poffset->distance);
                }
                else if(poffset && str_has_prefix(buffer_to_str(line), "m ")){
                    sscanf(buffer_to_str(line), "m %f"
                           ,&poffset->muscle);
                }
                else if(poffset && str_has_prefix(buffer_to_str(line), "ms ")){
                    sscanf(buffer_to_str(line), "ms %f"
                           ,&poffset->muscleSwell);
                }
                else if(poffset && str_has_prefix(buffer_to_str(line), "x ")){
                    f = 0.0f;
                    sscanf(buffer_to_str(line), "x %f"
                           ,&f);
                    
                    f = f * M_PI / 180.0f;
                    poffset->transform = c3d_matrix3_rotate_x(poffset->transform, f);
                }
                else if(poffset && str_has_prefix(buffer_to_str(line), "y ")){
                    f = 0.0f;
                    sscanf(buffer_to_str(line), "y %f"
                           ,&f);
                    
                    f = f * M_PI / 180.0f;
                    poffset->transform = c3d_matrix3_rotate_y(poffset->transform, f);
                }
                else if(poffset && str_has_prefix(buffer_to_str(line), "z ")){
                    f = 0.0f;
                    sscanf(buffer_to_str(line), "z %f"
                           ,&f);
                    
                    f = f * M_PI / 180.0f;
                    poffset->transform = c3d_matrix3_rotate_z(poffset->transform, f);
                }
            }
            buffer_clear(line);
            if(*p == '\0'){
                break;
            }
        }
        else{
            buffer_append(line, p, 1);
        }
        
        p ++;
    }
    
    buffer_dealloc(line);
}

void c3d_bone_anim_load_file(c3d_bone_context_t * context,c3d_bone_anim_t * anim,hcchar * filePath,InvokeTickDeclare){
    FILE * f = fopen(filePath, "r");
    hbuffer_t buf = buffer_alloc(MAX_READ_BUF, 128);
    hchar buffer[MAX_READ_BUF];
    hint32 len;
    if(f){
        
        while((len = fread(buffer, 1, sizeof(buffer), f)) >0){
            buffer_append(buf, buffer, len);
        }
        
        fclose(f);
        
        c3d_bone_anim_load(context,anim,buffer_to_str(buf),InvokeTickArg);
    }
    buffer_dealloc(buf);
}

c3d_bone_offset_t c3d_bone_offset_anim(c3d_bone_offset_t from,c3d_bone_offset_t to,double value,InvokeTickDeclare){
    c3d_bone_offset_t rs;
    memset(&rs,0,sizeof(c3d_bone_offset_t));
    rs.transform = c3d_matrix3_multiply_anim(from.transform,to.transform,value);
    rs.distance = from.distance + (to.distance - from.distance) * value;
    rs.muscle = from.muscle + (to.muscle - from.muscle) * value;
    rs.muscleSwell = from.muscleSwell + (to.muscleSwell - from.muscleSwell) * value;
    return rs;
}

void c3d_bone_anim_frame_add_offset(c3d_bone_anim_frame_t * frame,hint32 vIndex,c3d_bone_offset_t * offset,InvokeTickDeclare){
    if(frame->offsets == NULL){
        frame->offsets = serial_map_alloc(sizeof(hint32), sizeof(c3d_bone_offset_t), 0);
    }
    c3d_bone_offset_t * o = serial_map_get(frame->offsets, &vIndex);
    if(o){
        o->transform = c3d_matrix3_multiply(o->transform,offset->transform);
        o->distance = o->distance + offset->distance;
        o->muscle = o->muscle + offset->muscle;
        o->muscleSwell = o->muscleSwell + offset->muscleSwell;
    }
    else{
        serial_map_put(frame->offsets, &vIndex,offset);
    }
}


