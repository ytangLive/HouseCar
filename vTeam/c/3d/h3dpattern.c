//
//  h3dpattern.c
//  hc3d
//
//  Created by zhang hailong on 13-1-10.
//  Copyright (c) 2013年 hailong.org. All rights reserved.
//

#include "hconfig.h"
#include "h3dpattern.h"
#include "hbuffer.h"

#include "hserial_list.h"
#include "hserial_map.h"
#include "hdata_alloc.h"
#include "hstr.h"
#include "hlog.h"

#define IS_SPACE(c) ( (c) == ' ' || (c) == '\t' )
#define IS_LINE_END(c)  ( (c) == '\r' || (c) == '\n')

c3d_pattern_t c3d_pattern_load(const char * objSource,const char * mtlSource,float scale,c3d_vector3_t offset,InvokeTickDeclare){
    c3d_pattern_t rs;
    hdata_t mtlObject = hdata_object_alloc();
    hchar * p = (hchar *) mtlSource;
    hbuffer_t line = buffer_alloc(128, 128);
    str_range_t r = {0,0};
    c3d_pattern_material_t mtl, * pmtl = NULL;
    c3d_pattern_element_t element;
    c3d_pattern_element_range_t range, * prange;
    c3d_vector3_t v3;
    c3d_pattern_bone_t bone;
    hserial_list_t txtcoords = serial_list_alloc(sizeof(c3d_vector2_t), 0);
    hchar name[C3D_NAME_MAX] = {0};
    
    struct {
        unsigned int v;
        unsigned int vn;
        unsigned int vt;
    } face;
    
    memset(&rs, 0, sizeof(c3d_pattern_t));
    memset(&bone, 0, sizeof(c3d_pattern_bone_t));

    rs.muscleScale = 1.0;
    rs.materials = serial_list_alloc(sizeof(c3d_pattern_material_t), 0);
    rs.elements = serial_list_alloc(sizeof(c3d_pattern_element_t), 0);
    rs.element_ranges = serial_list_alloc(sizeof(c3d_pattern_element_range_t), 0);
    rs.vertexs = serial_list_alloc(sizeof(c3d_vector3_t), 0);
    rs.normals = serial_list_alloc(sizeof(c3d_vector3_t), 0);
    rs.bones = serial_list_alloc(sizeof(c3d_pattern_bone_t), 0);
    
    range.material = 0;
    range.location = 0;
    range.length = 0;
    
    serial_list_item_add(rs.element_ranges, &range);
    
    prange = (c3d_pattern_element_range_t *) serial_list_item_last(rs.element_ranges);
    
    while(p){
        
     
        r = str_find(p,'\r','\n',0);
        
        if(r.location >0){
            while(IS_SPACE(*p)){
                p++;
                r.location --;
            }
            
            if(strncmp(p, "newmtl",6) ==0){
                {
                    memset(name, 0, sizeof(name));
                    
                    buffer_clear(line);
                    buffer_append(line, p + 6 , r.location - 6);
                    sscanf(buffer_to_str(line),"%s", name);
                    
                    memset(&mtl, 0, sizeof(c3d_pattern_material_t));
                    
                    serial_list_item_add(rs.materials, &mtl);
                    
                    hdata_object_put(mtlObject, name, hdata_int32_alloc(serial_list_count(rs.materials)));
                    
                    pmtl = (c3d_pattern_material_t *) serial_list_item_last(rs.materials);
                    
                    hlog("newmtl %s\n",name);
                }
            }
            else if(strncmp(p, "map_d ",6) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p + 6, r.location - 6);
                    if(pmtl){
                       
                        sscanf(buffer_to_str(line), "%s",pmtl->map_d.texture);

                        hlog("map_d %s\n",pmtl->map_d.texture);
                    }
                }
            }
            else if(strncmp(p, "map_Kd ",7) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p + 7, r.location - 7);
                    if(pmtl){
                        
                        sscanf(buffer_to_str(line), "%s",pmtl->map_Kd.texture);
                        hlog("map_Kd %s\n",pmtl->map_Kd.texture);
                    }
                }
            }
            else if(strncmp(p, "map_Ka ",7) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p + 7, r.location - 7);
                    if(pmtl){
                        
                        sscanf(buffer_to_str(line), "%s",pmtl->map_Ka.texture);
                        hlog("map_Ka %s\n",pmtl->map_Ka.texture);
                    }
                }
            }
            else if(strncmp(p, "map_Ks ",7) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p + 7, r.location - 7);
                    if(pmtl){
                        
                        sscanf(buffer_to_str(line), "%s",pmtl->map_Ks.texture);
                        hlog("map_Ks %s\n",pmtl->map_Ks.texture);
                    }
                }
            }
            else if(strncmp(p, "Ka",2) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p + 2, r.location - 2);
                    
                    if(pmtl){
                        
                        pmtl->Ka.a = 1.0;
                        
                        sscanf(buffer_to_str(line), "%f %f %f",&pmtl->Ka.r,&pmtl->Ka.g,&pmtl->Ka.b);
                        
                        hlog("Ka %f %f %f \n",pmtl->Ka.r,pmtl->Ka.g,pmtl->Ka.b);
                        
                    }
                }
            }
            else if(strncmp(p, "Kd",2) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p +2, r.location -2);
                    
                    if(pmtl){
                        
                        pmtl->Kd.a = 1.0;
                        
                        sscanf(buffer_to_str(line), "%f %f %f",&pmtl->Kd.r,&pmtl->Kd.g,&pmtl->Kd.b);
                        
                        hlog("Kd %f %f %f \n",pmtl->Kd.r,pmtl->Kd.g,pmtl->Kd.b);
                        
                    }
                }
            }
            else if(strncmp(p, "Ks",2) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p + 2, r.location -2);
                    
                    if(pmtl){
                        
                        pmtl->Ks.a = 1.0;
                        
                        sscanf(buffer_to_str(line), "%f %f %f",&pmtl->Ks.r,&pmtl->Ks.g,&pmtl->Ks.b);
                        
                        hlog("Ks %f %f %f \n",pmtl->Ks.r,pmtl->Ks.g,pmtl->Ks.b);
                        
                    }
                }
            }
            else if(strncmp(p, "Ns",2) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p +2, r.location-2);
                    
                    if(pmtl){
                        
                        sscanf(buffer_to_str(line), "%f",&pmtl->Ns);
                        
                        hlog("Ns %f \n",pmtl->Ns);
                        
                    }
                }
            }
            else if(strncmp(p, "Tf ",3) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p +3, r.location-3);
                    
                    if(pmtl){
                        
                        pmtl->Tf.r = pmtl->Tf.g = pmtl->Tf.b = pmtl->Tf.a = 1.0;
                        
                        sscanf(buffer_to_str(line), "%f %f %f",&pmtl->Tf.r,&pmtl->Tf.g,&pmtl->Tf.b);
                        
                        hlog("Tf %f %f %f\n",pmtl->Tf.r,pmtl->Tf.g,pmtl->Tf.b);
                        
                    }
                }
            }
            else if(strncmp(p, "d ",2) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p +2, r.location-2);
                    
                    if(pmtl){
                        
                        pmtl->d = 1.0;
                        
                        sscanf(buffer_to_str(line), "%f",&pmtl->d);
                        
                        hlog("d %f \n",pmtl->d);
                        
                    }
                }
            }

        }
        else if(r.length ==0){
            break;
        }

        p += r.location + r.length;
        
        if(*p == '\0'){
            break;
        }
        
    }
    
    p = (hchar *) objSource;
    
    while(p){
        
        r = str_find(p,'\r','\n',0);
        
        if(r.location >0){
            while(IS_SPACE(*p)){
                p++;
                r.location --;
            }
            
            if(strncmp(p, "vn",2) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p +2, r.location -2);
                    
                    memset(&v3, 0, sizeof(c3d_vector3_t));
                    
                    sscanf(buffer_to_str(line), "%f %f %f",&v3.x,&v3.y,&v3.z);
                
                    serial_list_item_add(rs.normals, & v3);
                }
            }
            else if(strncmp(p, "vt",2) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p + 2, r.location -2);
                    
                    memset(&v3, 0, sizeof(c3d_vector3_t));
                    
                    sscanf(buffer_to_str(line), "%f %f",&v3.x,&v3.y);
                    
                    serial_list_item_add(txtcoords, & v3);
                    
                }
            }
            else if(strncmp(p, "v",1) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p + 1, r.location - 1);
                    
                    memset(&v3, 0, sizeof(c3d_vector3_t));
                    
                    sscanf(buffer_to_str(line), "%f %f %f",&v3.x,&v3.y,&v3.z);
                    
                    v3 = c3d_vector3_multiply_scalar(v3, scale);
                    
                    v3 = c3d_vector3_add(v3, offset);
                    
                    serial_list_item_add(rs.vertexs, & v3);
                    
                    serial_list_item_add(rs.bones, & bone);
                    
                    rs.minVertex.x = MIN(v3.x, rs.minVertex.x);
                    rs.minVertex.y = MIN(v3.y, rs.minVertex.y);
                    rs.minVertex.z = MIN(v3.z, rs.minVertex.z);
                    
                    rs.maxVertex.x = MAX(v3.x, rs.maxVertex.x);
                    rs.maxVertex.y = MAX(v3.y, rs.maxVertex.y);
                    rs.maxVertex.z = MAX(v3.z, rs.maxVertex.z);
                }
            }
            else if(strncmp(p, "f",1) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p +1, r.location -1);
                    memset(&face, 0, sizeof(face));
                    
                    sscanf(buffer_to_str(line), "%d%*[/]%d%*[/]%d",&face.v,&face.vt,&face.vn);
                    
                    if(face.vn == 0){
                        face.vn = face.vt;
                        face.vt = 0;
                    }
                    
                    memset(&element, 0, sizeof(element));
                    
                    if(face.v){
                        element.vertex = * (c3d_vector3_t *) serial_list_item_at(rs.vertexs, face.v - 1);
                        element.vIndex = face.v;
                    }
                    
                    if(face.vn){
                        element.normal = * (c3d_vector3_t *) serial_list_item_at(rs.normals, face.vn -1);
                        element.vnIndex = face.vn;
                    }
                    
                    if(face.vt){
                        element.txtcoord = * (c3d_vector2_t *) serial_list_item_at(txtcoords, face.vt - 1);
                    }
                    
                    serial_list_item_add(rs.elements, &element);
                    
                    if(prange){
                        prange->length ++;
                    }
                    
                    memset(&face, 0, sizeof(face));
                    
                    sscanf(buffer_to_str(line), "%*s%d%*[/]%d%*[/]%d",&face.v,&face.vt,&face.vn);
                    
                    if(face.vn == 0){
                        face.vn = face.vt;
                        face.vt = 0;
                    }
                    
                    memset(&element, 0, sizeof(element));
                    
                    if(face.v){
                        element.vertex = * (c3d_vector3_t *) serial_list_item_at(rs.vertexs, face.v - 1);
                        element.vIndex = face.v;
                    }
                    
                    if(face.vn){
                        element.normal = * (c3d_vector3_t *) serial_list_item_at(rs.normals, face.vn -1);
                        element.vnIndex = face.vn;
                    }
                    
                    if(face.vt){
                        element.txtcoord = * (c3d_vector2_t *) serial_list_item_at(txtcoords, face.vt - 1);
                    }
                    
                    serial_list_item_add(rs.elements, &element);
                    
                    if(prange){
                        prange->length ++;
                    }
                    
                    memset(&face, 0, sizeof(face));
                    
                    sscanf(buffer_to_str(line), "%*s%*s%d%*[/]%d%*[/]%d",&face.v,&face.vt,&face.vn);
                    
                    if(face.vn == 0){
                        face.vn = face.vt;
                        face.vt = 0;
                    }
                    
                    memset(&element, 0, sizeof(element));
                    
                    if(face.v){
                        element.vertex = * (c3d_vector3_t *) serial_list_item_at(rs.vertexs, face.v - 1);
                        element.vIndex = face.v;
                    }
                    
                    if(face.vn){
                        element.normal = * (c3d_vector3_t *) serial_list_item_at(rs.normals, face.vn -1);
                        element.vnIndex = face.vn;
                    }
                    
                    if(face.vt){
                        element.txtcoord = * (c3d_vector2_t *) serial_list_item_at(txtcoords, face.vt - 1);
                    }
                    
                    serial_list_item_add(rs.elements, &element);
                    
                    if(prange){
                        prange->length ++;
                    }
                    
                }
            }
            else if(strncmp(p, "usemtl",6) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p +6, r.location -6);
                    
                    memset(name,0,sizeof(name));
                    
                    sscanf(buffer_to_str(line), "%s",name);
                    
                    if(prange){
                        if(prange->material != 0 || prange->length !=0){
                            range.location = prange->location + prange->length;
                            range.length = 0;
                            range.material = 0;
                            serial_list_item_add(rs.element_ranges, &range);
                            prange = (c3d_pattern_element_range_t *) serial_list_item_last(rs.element_ranges);
                        }
                    }
                    else{
                        range.location = 0;
                        range.length = 0;
                        range.material = 0;
                        serial_list_item_add(rs.element_ranges, &range);
                        prange = (c3d_pattern_element_range_t *) serial_list_item_last(rs.element_ranges);
                    }
                    
                    hdata_t v = hdata_object(&hdata_class, mtlObject, name);
                    if(v){
                        prange->material = hdata_int32(&hdata_class, v, 0);
                    }
                }
            }
            
        }
        else if(r.length ==0){
            break;
        }
        
        p += r.location + r.length;
        
        if(*p == '\0'){
            break;
        }
    }
    
    hdata_dealloc(mtlObject);
    buffer_dealloc(line);
    serial_list_dealloc(txtcoords);
    
    return rs;
}

c3d_pattern_t c3d_pattern_load_file(const char * objFilePath,const char * mtlFilePath,float scale,c3d_vector3_t offset, InvokeTickDeclare){
    
    hbuffer_t objSource = buffer_alloc(512, 128);
    hbuffer_t mtlSource = buffer_alloc(512, 128);
    FILE * f = fopen(objFilePath, "r");
    hchar buffer[512];
    hint32 len;
    c3d_pattern_t p;
    
    if(f){
        while(!feof(f) && (len = fread(buffer, 1, sizeof(buffer), f)) >0 ){
            buffer_append(objSource, buffer, len);
        }
        fclose(f);
    }
    
    if(mtlFilePath){
    
        f = fopen(mtlFilePath, "r");
        
        if(f){
            while(!feof(f) && (len = fread(buffer, 1, sizeof(buffer), f)) >0 ){
                buffer_append(mtlSource, buffer, len);
            }
            fclose(f);
        }
    }
    
    p = c3d_pattern_load(buffer_to_str(objSource), buffer_to_str(mtlSource), scale,offset, InvokeTickArg);
    
    buffer_dealloc(objSource);
    buffer_dealloc(mtlSource);
    
    return p;
}

void c3d_pattern_bone_bind(c3d_pattern_t * pattern,const char * boneSource, c3d_bone_context_t * boneContext,InvokeTickDeclare){
    
    hbuffer_t line = buffer_alloc(128, 128);
    hchar * p = (hchar *) boneSource;
    str_range_t r;
    hchar name[C3D_NAME_MAX];
    hint32 boneIndex = 0,i,vi1,vi2,s = 0;
    hfloat weight = 0.0;
    c3d_pattern_bone_t * pbone;
    hserial_list_t vertexs = serial_list_alloc(sizeof(hint32), 0);
    c3d_vector3_t min,max,v3;
    
    
    while(p){
        
        r = str_find(p,'\r','\n',0);
        
        if(r.location >0){
            while(IS_SPACE(*p)){
                p++;
                r.location --;
            }
            
            if(strncmp(p, "vs",2) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p + 2, r.location - 2);
                   
                    vi1 = vi2 = 0;
                    
                    sscanf(buffer_to_str(line), "%d~%d",&vi1,&vi2);
                    
                    if(s == 1){
                        serial_list_clear(vertexs);
                        s = 0;
                    }
                    
                    if(vi1 && vi2 && vi1 <= vi2){
                        for(i=vi1;i<=vi2;i++){
                            serial_list_item_add(vertexs, &i);
                        }
                    }
                                
                }
            }
            else if(strncmp(p, "v",1) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p +1, r.location -1);
                    
                    vi1 =0;
                    
                    sscanf(buffer_to_str(line), "%d",&vi1);
                    
                    if(s == 1){
                        serial_list_clear(vertexs);
                        s = 0;
                    }
                    
                    
                    if(vi1){
                        serial_list_item_add(vertexs, &vi1);
                    }
                    
                }
            }
            else if(strncmp(p, "r",1) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p + 1, r.location - 1);
                    
                    memset(&min, 0, sizeof(min));
                    memset(&max, 0, sizeof(max));
                    
                    sscanf(buffer_to_str(line), "%f %f %f %f %f %f"
                           ,&min.x,&min.y,&min.z
                           ,&max.x,&max.y,&max.z);
                    
                    if(s == 1){
                        serial_list_clear(vertexs);
                        s = 0;
                    }
                    
                    for(i=0;i<serial_list_count(pattern->vertexs);i++){
                        v3 = * (c3d_vector3_t *) serial_list_item_at(pattern->vertexs, i);
                        if(v3.x >= min.x && v3.x <= max.x
                           && v3.y >= min.y && v3.y <= max.y
                           && v3.z >= min.z && v3.z <= max.z){
                            vi1 = i +1;
                            serial_list_item_add(vertexs, &vi1);
                        }
                    }
                }
            }
            else if(strncmp(p, "b",1) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p + 1, r.location - 1);
                    
                    memset(name, 0, sizeof(name));
                    
                    weight = 1.0;
                    
                    sscanf(buffer_to_str(line), "%s %f",name,&weight);
                    
                    boneIndex = c3d_bone_context_index_for(boneContext,name,InvokeTickArg) + 1;
                    
                    if(boneIndex){
                        for(i=0;i<serial_list_count(vertexs);i++){
                            vi1 = * (hint32 *) serial_list_item_at(vertexs, i);
                            pbone = serial_list_item_at(pattern->bones, vi1 -1);
                            memset(pbone, 0, sizeof(c3d_pattern_bone_t));
                            pbone->link = boneIndex;
                            pbone->weight = weight;
                        }
                    }
                    
                    s = 1;
                }
            }
            
            
        }
        else if(r.length ==0){
            break;
        }
        
        p += r.location + r.length;
        
        if(*p == '\0'){
            break;
        }
    }
    
    buffer_dealloc(line);
    serial_list_dealloc(vertexs);
}

void c3d_pattern_bone_bind_file(c3d_pattern_t * pattern,const char * boneFilePath, c3d_bone_context_t * boneContext,InvokeTickDeclare){
    hbuffer_t source = buffer_alloc(512, 128);
    FILE * f = fopen(boneFilePath, "r");
    hchar buffer[512];
    hint32 len;
    
    if(f){
        while(!feof(f) && (len = fread(buffer, 1, sizeof(buffer), f)) >0 ){
            buffer_append(source, buffer, len);
        }
        fclose(f);
    }
    
    c3d_pattern_bone_bind(pattern,buffer_to_str(source),boneContext, InvokeTickArg);
    
    buffer_dealloc(source);
    
    return ;
}

typedef struct _c3d_pattern_bone_auto_t{
    unsigned int link;
    float length;
    float weight;
}c3d_pattern_bone_auto_t;

void c3d_pattern_bone_bind_auto(c3d_pattern_t * pattern, c3d_bone_context_t * boneContext,InvokeTickDeclare){
    hint32 i,c = serial_list_count(pattern->vertexs);
    hint32 n,nc = serial_list_count(boneContext->bones);
    c3d_bone_t * bone;
    c3d_vector3_t vertex,v1,v2 ;
    c3d_pattern_bone_t * pbone;
    c3d_pattern_bone_auto_t autoBone;
    hfloat v,d,e;
    
    for(i=0;i<c;i++){
        vertex = * (c3d_vector3_t *) serial_list_item_at(pattern->vertexs, i);
        pbone = (c3d_pattern_bone_t *) serial_list_item_at(pattern->bones, i);
        
        memset(&autoBone, 0, sizeof(autoBone));
        
        for(n=0;n<nc;n++){
            
            bone = (c3d_bone_t *)serial_list_item_at(boneContext->bones, n);
            
            if(bone->range.maxVector.x - bone->range.minVector.x >0){
                if(vertex.x < bone->range.minVector.x || vertex.x > bone->range.maxVector.x){
                    continue;
                }
            }
            
            if(bone->range.maxVector.y - bone->range.minVector.y >0){
                if(vertex.y < bone->range.minVector.y || vertex.y > bone->range.maxVector.y){
                    continue;
                }
            }
            
            if(bone->range.maxVector.z - bone->range.minVector.z >0){
                if(vertex.z < bone->range.minVector.z || vertex.z > bone->range.maxVector.z){
                    continue;
                }
            }
            
            v1 = * ( c3d_vector3_t * ) serial_list_item_at(boneContext->positions, n + 1);
            v2 = * ( c3d_vector3_t * ) serial_list_item_at(boneContext->positions, bone->parentIndex);
            v = c3d_vector3_project_scalar(v1, v2, vertex);
            if(v >= bone->minAffect && v<= bone->maxAffect){
                d = c3d_vector3_length(c3d_vector3_sub(vertex, c3d_vector3_anim(v1, v2, v)));
                if(d <= bone->size){
                    if(autoBone.link == 0 || d < autoBone.length ){
                        autoBone.link = n +1;
                        autoBone.length = d;
                        if(bone->elbow == 0.0f){
                            autoBone.weight = 1.0;
                        }
                        else if((e = bone->maxAffect - v) <= bone->elbow){
                            autoBone.weight = 0.5 + e / bone->elbow * 0.5;
                        }
                        else{
                            autoBone.weight = 1.0;
                        }
                    }
                }
            }
        }
        
        if(autoBone.link >0){
            pbone->link = autoBone.link;
            pbone->weight = autoBone.weight;
        }
        
    }
}

void c3d_pattern_bond_apply(c3d_pattern_t * pattern,c3d_bone_context_t * boneConetxt,InvokeTickDeclare){
    hint32 i,c = serial_list_count(pattern->elements);
    c3d_pattern_element_t * element;
    c3d_pattern_bone_t * bone;
    hint32 vIndex,vnIndex,ii;
    c3d_vector3_t v1,v2,v3,v5,vp;
    c3d_matrix3_t transform;
    hfloat v,distance,muscle ,scalar,muscleSwall;
    huint32 link;
    c3d_bone_t * b;
    
    for(i=0;i<c;i++){
        element = (c3d_pattern_element_t *) serial_list_item_at(pattern->elements, i);
        vIndex =  element->vIndex;
        vnIndex = element->vnIndex;
        
        element->vertex = * ( c3d_vector3_t *) serial_list_item_at(pattern->vertexs, vIndex - 1);
        element->normal = * ( c3d_vector3_t *) serial_list_item_at(pattern->normals, vnIndex - 1);
        
        bone = (c3d_pattern_bone_t *) serial_list_item_at(pattern->bones, vIndex -1);
    
        
        distance = 0;
        ii = 0;
        
        if(bone && bone->link > 0){

            link = bone->link;

            v = bone->weight;
            
            b = serial_list_item_at(boneConetxt->bones, link - 1);
            
            v2 = * (c3d_vector3_t *) serial_list_item_at(boneConetxt->positions, link);
            vp = * (c3d_vector3_t *) serial_list_item_at(boneConetxt->positions, b->parentIndex);
            v1 = * (c3d_vector3_t * ) serial_list_item_at(boneConetxt->positions_anim, b->parentIndex);
            transform = * (c3d_matrix3_t *) serial_list_item_at(boneConetxt->transforms_anim, link);
            distance = * (float *) serial_list_item_at(boneConetxt->distances, link);
            muscle = * (float *) serial_list_item_at(boneConetxt->muscles, link);
            muscleSwall = * (float *) serial_list_item_at(boneConetxt->muscleSwells, link);
            
            muscle += b->muscle;
            
            v5 =  c3d_vector3_multiply_scalar(c3d_vector3_sub(element->vertex, vp), (1.0 + distance));
            
            if(muscleSwall != 0.0){
                
                scalar = c3d_vector3_project_scalar(vp, v2, element->vertex);
                
                muscleSwall = pattern->muscleScale * muscleSwall * (1.0  - fabsf(scalar - muscle));

                v3 = c3d_vector3_anim(c3d_vector3_anim(vp, v2, scalar), element->vertex, 1.0 + muscleSwall );
                v3 = c3d_vector3_sub(v3, element->vertex);

                v5 = c3d_vector3_add(v5, v3);
            }
            
            if(v < 1.0f){
                transform = c3d_matrix3_multiply_anim(c3d_matrix3_identity, transform, v);
            }
            
            v5 =  c3d_matrix3_multiply_vector3(transform,v5 );
            
            v5 =  c3d_vector3_add(v1, v5);
            
            element->normal = c3d_matrix3_multiply_vector3(transform, element->normal);
            
            element->vertex = v5;

            
        }

    }
}

void c3d_pattern_delete(c3d_pattern_t * pattern,InvokeTickDeclare){
    serial_list_dealloc(pattern->elements);
    serial_list_dealloc(pattern->element_ranges);
    serial_list_dealloc(pattern->materials);
    serial_list_dealloc(pattern->vertexs);
    serial_list_dealloc(pattern->bones);
    serial_list_dealloc(pattern->normals);
    memset(pattern,0,sizeof(c3d_pattern_t));
}

void c3d_pattern_light_load(c3d_pattern_light_t * light,const char * lightSource,InvokeTickDeclare){
    str_range_t r;
    hchar * p = (hchar *) lightSource;
    hbuffer_t line = buffer_alloc(128, 128);
    hint32 index = 0;
    
    while(p){
        
        r = str_find(p,'\r','\n',0);
        
        if(r.location >0){
            while(IS_SPACE(*p)){
                p++;
                r.location --;
            }
            
            if(strncmp(p, "-l ",2) ==0){
                {
                    buffer_clear(line);
                    buffer_append(line, p +2, r.location -2);

                    sscanf(buffer_to_str(line), "%f %f %f %f",&light->location[index].x,&light->location[index].y,&light->location[index].z,&light->location[index].w);
                    
                    if( ++ index >= C3D_MAX_PATTERN_LIGHT_SIZE){
                        break;
                    }
                }
            }           
        }
        else if(r.length ==0){
            break;
        }
        
        p += r.location + r.length;
        
        if(*p == '\0'){
            break;
        }
        if( index >= C3D_MAX_PATTERN_LIGHT_SIZE){
            break;
        }
    }

    buffer_dealloc(line);
}

void c3d_pattern_light_load_file(c3d_pattern_light_t * light,const char * lightFile,InvokeTickDeclare){
    hbuffer_t source = buffer_alloc(512, 128);
    FILE * f = fopen(lightFile, "r");
    hchar buffer[512];
    hint32 len;
    
    if(f){
        while(!feof(f) && (len = fread(buffer, 1, sizeof(buffer), f)) >0 ){
            buffer_append(source, buffer, len);
        }
        fclose(f);
    }
    
    c3d_pattern_light_load(light,buffer_to_str(source), InvokeTickArg);
    
    buffer_dealloc(source);
}
