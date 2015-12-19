//
//  h3dmodel.c
//  hc3d
//
//  Created by Zhang Hailong on 12-11-10.
//
//

#include "hconfig.h"
#include "h3dmodel.h"
#include "hserial_list.h"
#include "hfile.h"
#include "hbuffer.h"
#include "hdata_alloc.h"
#include "hstr.h"
#include "h3dcontext.h"
#include "hlog.h"

static void c3d_model_dealloc_fun(c3d_model_t * model){
    InvokeTickBegin
    if(model->data){
        free(model->data);
    }
    hdata_dealloc(model->groups);
    hdata_dealloc(model->mtls);
}

void c3d_model_delete(c3d_model_t model){
    if(model.dealloc){
        (* model.dealloc)(&model);
    }
}

static void c3d_model_load_mtl(hdata_t mtls ,hcchar * mtlContent){
    
    InvokeTickBegin
    hchar * p = (hchar *) mtlContent;
    hint32 s = 0;
    hbuffer_t line = buffer_alloc(128, 128);
    hbuffer_t cmd = buffer_alloc(20, 20);
    hbuffer_t key = buffer_alloc(128, 128);
    c3d_model_mtl_t mtl , * pmtl;
    
    while(*p != '\0'){
        //hlog("%c",* p);
        if(s ==0){
            if(* p == '\r' || * p == '\n' ){
                buffer_clear(cmd);
            }
            else if(* p == ' ' || * p == '\t'){
                if(buffer_length(cmd) >0){
                    if(strcmp(buffer_to_str(cmd), "newmtl") ==0){
                        buffer_clear(line);
                        buffer_clear(cmd);
                        buffer_clear(key);
                        s = 1;
                    }
                    else if(strcmp(buffer_to_str(cmd), "map_d") ==0 || strcmp(buffer_to_str(cmd), "map_Kd") ==0){
                        buffer_clear(line);
                        buffer_clear(cmd);
                        s = 2;
                    }
                    else if(strcmp(buffer_to_str(cmd), "Ka") ==0){
                        buffer_clear(line);
                        buffer_clear(cmd);
                        s = 3;
                    }
                    else if(strcmp(buffer_to_str(cmd), "Kd") ==0){
                        buffer_clear(line);
                        buffer_clear(cmd);
                        s = 4;
                    }
                    else if(strcmp(buffer_to_str(cmd), "Ks") ==0){
                        buffer_clear(line);
                        buffer_clear(cmd);
                        s = 5;
                    }
                    else if(strcmp(buffer_to_str(cmd), "Ns") ==0){
                        buffer_clear(line);
                        buffer_clear(cmd);
                        s = 6;
                    }
                    else if(strcmp(buffer_to_str(cmd), "#") ==0){
                        buffer_clear(line);
                        buffer_clear(cmd);
                        s = 10;
                    }
                }
            }
            else{
                buffer_append(cmd, p, 1);
            }
        }
        else if(s == 1){
            // newmtl 46ade6ff_dds
            if(* p == '\r' || * p == '\n'){
                hlog("newmtl %s\n",buffer_to_str(key));
                memset(&mtl, 0, sizeof(c3d_model_mtl_t));
                if(buffer_length(key)){
                    hdata_object_put(mtls, buffer_to_str(key), hdata_bytes_alloc((hbyte *)&mtl, sizeof(c3d_model_mtl_t)));
                }
                s = 0;
                if(p[1] =='\n'){
                    p++;
                }
            }
            else if(* p == ' ' || * p == '\t'){
                
            }
            else{
                buffer_append(key, p, 1);
            }
        }
        else if(s == 2){
            // map_d -o -0.2430 -0.4900 0.0000 -s 0.6800 0.5100 1.0000 uv.png
            if(* p == '\r' || * p == '\n'){
                pmtl = (c3d_model_mtl_t *)hdata_bytes(&hdata_class, hdata_object(&hdata_class,mtls,buffer_to_str(key)));
                if(pmtl){
                    pmtl->map_d.size.x = 1.0;
                    pmtl->map_d.size.y = 1.0;
                    pmtl->map_d.size.z = 1.0;
                    if(str_exist(buffer_to_str(line), "-o")){
                        sscanf(buffer_to_str(line), "-o %f %f %f -s %f %f %f %s",&pmtl->map_d.position.x,&pmtl->map_d.position.y,&pmtl->map_d.position.z
                               ,&pmtl->map_d.size.x,&pmtl->map_d.size.y,&pmtl->map_d.size.z,pmtl->map_d.texture);
                    }
                    else{
                        sscanf(buffer_to_str(line), "%s",pmtl->map_d.texture);
                    }
                    
                    hlog("map_d -o %f %f %f -s %f %f %f %s\n",pmtl->map_d.position.x,pmtl->map_d.position.y,pmtl->map_d.position.z
                         ,pmtl->map_d.size.x,pmtl->map_d.size.y,pmtl->map_d.size.z,pmtl->map_d.texture);
                    
                }
                s = 0;
                if(p[1] =='\n'){
                    p++;
                }
            }
            else{
                buffer_append(line, p, 1);
            }
        }
        else if(s == 3){
            // Ka 0.000000000000 0.000000000000 0.000000000000
            if(* p == '\r' || * p == '\n'){
                pmtl = (c3d_model_mtl_t *)hdata_bytes(&hdata_class, hdata_object(&hdata_class,mtls,buffer_to_str(key)));
                if(pmtl){

                    pmtl->Ka.a = 1.0;
                    
                    sscanf(buffer_to_str(line), "%f %f %f",&pmtl->Ka.r,&pmtl->Ka.g,&pmtl->Ka.b);
                
                    hlog("Ka %f %f %f \n",pmtl->Ka.r,pmtl->Ka.g,pmtl->Ka.b);
                    
                }
                
                s = 0;
                if(p[1] =='\n'){
                    p++;
                }
            }
            else{
                buffer_append(line, p, 1);
            }
        }
        else if(s == 4){
            // Kd 0.000000000000 0.000000000000 0.000000000000
            if(* p == '\r' || * p == '\n'){
                pmtl = (c3d_model_mtl_t *)hdata_bytes(&hdata_class, hdata_object(&hdata_class,mtls,buffer_to_str(key)));
                if(pmtl){
                    
                    pmtl->Kd.a = 1.0;
                    
                    sscanf(buffer_to_str(line), "%f %f %f",&pmtl->Kd.r,&pmtl->Kd.g,&pmtl->Kd.b);
                    
                    hlog("Kd %f %f %f \n",pmtl->Kd.r,pmtl->Kd.g,pmtl->Kd.b);
                    
                }
                
                s = 0;
                if(p[1] =='\n'){
                    p++;
                }
            }
            else{
                buffer_append(line, p, 1);
            }
        }
        else if(s == 5){
            // Ks 0.000000000000 0.000000000000 0.000000000000
            if(* p == '\r' || * p == '\n'){
                pmtl = (c3d_model_mtl_t *)hdata_bytes(&hdata_class, hdata_object(&hdata_class,mtls,buffer_to_str(key)));
                if(pmtl){
                    
                    pmtl->Ka.s = 1.0;
                    
                    sscanf(buffer_to_str(line), "%f %f %f",&pmtl->Ks.r,&pmtl->Ks.g,&pmtl->Ks.b);
                    
                    hlog("Kd %f %f %f \n",pmtl->Ks.r,pmtl->Ks.g,pmtl->Ks.b);
                    
                }
                
                s = 0;
                if(p[1] =='\n'){
                    p++;
                }
            }
            else{
                buffer_append(line, p, 1);
            }
        }
        else if(s == 6){
            // Ns 32
            if(* p == '\r' || * p == '\n'){
                pmtl = (c3d_model_mtl_t *)hdata_bytes(&hdata_class, hdata_object(&hdata_class,mtls,buffer_to_str(key)));
                if(pmtl){
                    
                    sscanf(buffer_to_str(line), "%f",&pmtl->Ns);
                    
                    hlog("Ns %f \n",pmtl->Ns);
                    
                }
                
                s = 0;
                if(p[1] =='\n'){
                    p++;
                }
            }
            else{
                buffer_append(line, p, 1);
            }
        }
        else if(s == 10){
            if(* p == '\r' || * p == '\n'){
                s = 0;
                if(p[1] =='\n'){
                    p++;
                }
            }
        }
        p++;
    }

    buffer_dealloc(cmd);
    buffer_dealloc(line);
    buffer_dealloc(key);
    
}

c3d_model_t c3d_model_load_obj(struct _c3d_context_t * context,hcchar * objFile,hcchar * mtlFile,float scale,c3d_bone_context_t * boneContext){
    InvokeTickBegin
    c3d_model_t m = {NULL,0,C3D_MODEL_MASK_VERTEXT | C3D_MODEL_MASK_NORMAL | C3D_MODEL_MASK_TXT_COORD,{0.0,0.0,0.0},{0.0,0.0,0.0},c3d_model_dealloc_fun,hdata_object_alloc(),hdata_object_alloc()};
    
    struct {
        c3d_model_vertex_t v;
        c3d_model_normal_t vn;
        c3d_model_txtcoord_t t;
    } item;

    hserial_list_t vertexts = serial_list_alloc(sizeof(c3d_model_vertex_t), 0);
    hserial_list_t normals = serial_list_alloc(sizeof(c3d_model_normal_t), 0);
    hserial_list_t txtCoords = serial_list_alloc(sizeof(c3d_model_txtcoord_t), 0);
    hserial_list_t items = serial_list_alloc(sizeof(item), 0);
    hint32 ii;
    hint32 s = 0;
    c3d_model_vertex_t v;
    c3d_model_normal_t vn;
    c3d_model_txtcoord_t t;
    hbuffer_t line = buffer_alloc(128, 128);
    hbuffer_t cmd = buffer_alloc(20, 20);
    hdata_t mtl = NULL;
    hdata_t group = NULL;
    c3d_model_group_t g,* pg;
    c3d_model_mtl_t * pmtl = NULL;
    hbuffer_t content = buffer_alloc(1024, 1024);
    hchar * p = NULL;
    
    struct {
        hint32 vi,vni,ti;
    } vf[3];
    

    if(mtlFile){
        if(c3d_context_resource_content(context, mtlFile, content, InvokeTickArg) >0){
            c3d_model_load_mtl(m.mtls, buffer_to_str(content));
        }
    }
    
    buffer_clear(content);
    
    c3d_context_resource_content(context, objFile, content, InvokeTickArg);
    
    p = (hchar *)buffer_to_str(content);
    
    while(*p != '\0'){
        //hlog("%c",* p);
        if(s ==0){
            if(* p == '\r' || * p == '\n' ){
                buffer_clear(cmd);
            }
            else if(* p == ' ' ){
                if(strcmp(buffer_to_str(cmd), "vn") ==0){
                    buffer_clear(line);
                    buffer_clear(cmd);
                    s = 2;
                }
                else if(strcmp(buffer_to_str(cmd), "vt") ==0){
                    buffer_clear(line);
                    buffer_clear(cmd);
                    s = 3;
                }
                else if(strcmp(buffer_to_str(cmd), "v") ==0){
                    buffer_clear(line);
                    buffer_clear(cmd);
                    s = 1;
                }
                else if(strcmp(buffer_to_str(cmd), "f") ==0){
                    buffer_clear(line);
                    buffer_clear(cmd);
                    s = 4;
                }
                else if(strcmp(buffer_to_str(cmd), "usemtl") ==0){
                    buffer_clear(line);
                    buffer_clear(cmd);
                    s = 5;
                }
                else if(strcmp(buffer_to_str(cmd), "g") ==0){
                    buffer_clear(line);
                    buffer_clear(cmd);
                    s = 6;
                }
                else if(strcmp(buffer_to_str(cmd), "bone") ==0){
                    buffer_clear(line);
                    buffer_clear(cmd);
                    s = 7;
                }
                else if(strcmp(buffer_to_str(cmd), "#") ==0){
                    buffer_clear(line);
                    buffer_clear(cmd);
                    s = 10;
                }
            }
            else{
                buffer_append(cmd, p, 1);
            }
        }
        else if(s == 1){
            // v -51.0374 6.11224 102.908
            if(* p == '\r' || * p == '\n'){
                memset(&v, 0, sizeof(c3d_model_vertex_t));
                sscanf(buffer_to_str(line), "%f %f %f",&v.x,&v.y,&v.z);
                v.x *= scale;
                v.y *= scale;
                v.z *= scale;
                serial_list_item_add(vertexts, &v);
                s = 0;
                if(p[1] =='\n'){
                    p++;
                }
            }
            else{
                buffer_append(line, p, 1);
            }
        }
        else if(s == 2){
            // vn -51.0374 6.11224 102.908
            if(* p == '\r' || * p == '\n'){
                memset(&vn, 0, sizeof(c3d_model_normal_t));
                sscanf(buffer_to_str(line), "%f %f %f",&vn.x,&vn.y,&vn.z);
                serial_list_item_add(normals, &vn);
                s = 0;
                if(p[1] =='\n'){
                    p++;
                }
            }
            else{
                buffer_append(line, p, 1);
            }
        }
        else if(s == 3){
            // vt 0.485157 0.130728 0.0
            if(* p == '\r' || * p == '\n'){
                memset(&t, 0, sizeof(c3d_model_txtcoord_t));
                sscanf(buffer_to_str(line), "%f %f %f",&t.x,&t.y,&t.z);
                serial_list_item_add(txtCoords, &t);
                s = 0;
                if(p[1] =='\n'){
                    p++;
                }
            }
            else{
                buffer_append(line, p, 1);
            }
        }
        else if(s == 4){
            // f 1/2/3
            if(* p == '\r' || * p == '\n'){
                
                memset(vf, 0, sizeof(vf));
                
                
                sscanf(buffer_to_str(line), "%d%*[/]%d%*[/]%d",&vf[0].vi,&vf[0].ti,&vf[0].vni);
                sscanf(buffer_to_str(line), "%*s%d%*[/]%d%*[/]%d",&vf[1].vi,&vf[1].ti,&vf[1].vni);
                sscanf(buffer_to_str(line), "%*s%*s%d%*[/]%d%*[/]%d",&vf[2].vi,&vf[2].ti,&vf[2].vni);
                
                for(ii=0;ii<3;ii++){
                    
                    memset(&item, 0, sizeof(item));
                    
                    if(vf[ii].vni == 0){
                        vf[ii].vni = vf[ii].ti;
                        vf[ii].ti = 0;
                    }
                    
                    assert(vf[ii].vi >=0 && vf[ii].vi <= serial_list_count(vertexts));
                    assert(vf[ii].vni >=0 && vf[ii].vni <= serial_list_count(normals));
                    assert( vf[ii].ti <= serial_list_count(txtCoords));
                    
                    memcpy(&item.v, serial_list_item_at(vertexts, vf[ii].vi - 1), sizeof(c3d_model_vertex_t));
                    memcpy(&item.vn, serial_list_item_at(normals, vf[ii].vni -1), sizeof(c3d_model_normal_t));
                    if(vf[ii].ti == 0){
                        item.t.x = -1;
                        item.t.y = -1;
                        item.t.z = 1;
                        
                        if(pmtl){
                            item.t.z =  item.t.z * pmtl->map_d.size.p  + pmtl->map_d.position.z;
                        }
                    }
                    else{
                        
                        memcpy(&item.t, serial_list_item_at(txtCoords, vf[ii].ti -1), sizeof(c3d_model_txtcoord_t));
                        
                        if(pmtl){
                            
                            item.t.x =  item.t.x * pmtl->map_d.size.s  + pmtl->map_d.position.x ;
                            
                            item.t.y =  item.t.y * pmtl->map_d.size.t  + pmtl->map_d.position.y;
                            
                            item.t.z =  item.t.z * pmtl->map_d.size.p  + pmtl->map_d.position.z;
                        }
                    }
                    
                    //                            hlog("{%f,%f,%f} {%f,%f,%f} {%f,%f,%f}\n",item.v.x,item.v.y,item.v.z
                    //                                   ,item.vn.x,item.vn.y,item.vn.z
                    //                                   ,item.t.x,item.t.y,item.t.z);
                    
                    if(item.v.x < m.minVertex.x){
                        m.minVertex.x = item.v.x;
                    }
                    if(item.v.x > m.maxVertex.x){
                        m.maxVertex.x = item.v.x;
                    }
                    if(item.v.y < m.minVertex.y){
                        m.minVertex.y = item.v.y;
                    }
                    if(item.v.y > m.maxVertex.y){
                        m.maxVertex.y = item.v.y;
                    }
                    if(item.v.z < m.minVertex.z){
                        m.minVertex.z = item.v.z;
                    }
                    if(item.v.z > m.maxVertex.z){
                        m.maxVertex.z = item.v.z;
                    }
                    
                    serial_list_item_add(items, &item);
                    
                    if(group){
                        pg = (c3d_model_group_t *)hdata_bytes(&hdata_class, group);
                        pg->range.length ++;
                    }
                }
                
                
                s = 0;
                
                if(p[1] =='\n'){
                    p++;
                }
            }
            else{
                buffer_append(line, p, 1);
            }
        }
        else if(s == 5){
            //usemtl 46ade6ff_dds
            if(* p == '\r' || * p == '\n'){
                
                mtl = hdata_object(&hdata_class, m.mtls, buffer_to_str(line));
                if(mtl){
                    pmtl = (c3d_model_mtl_t *)hdata_bytes(&hdata_class, mtl);
                    
                    if(group){
                        pg = (c3d_model_group_t *)hdata_bytes(&hdata_class, group);
                        strncpy(pg->mtl, buffer_to_str(line), sizeof(pg->mtl) -1);
                    }
                    
                }
                else{
                    pmtl = NULL;
                }
                s = 0;
                if(p[1] =='\n'){
                    p++;
                }
            }
            else if(* p == ' ' || * p == '\t'){
                
            }
            else{
                buffer_append(line, p, 1);
            }
        }
        else if(s == 6){
            //g DrawCall_0000
            if(* p == '\r' || * p == '\n'){
                memset(&g, 0, sizeof(c3d_model_group_t));
                g.range.offset = serial_list_count(items);
                group = hdata_bytes_alloc((hbyte *)&g, sizeof(c3d_model_group_t));
                hdata_object_put(m.groups, buffer_to_str(line), group);
                hlog("g %s\n",buffer_to_str(line));
                s = 0;
                if(p[1] =='\n'){
                    p++;
                }
            }
            else if(* p == ' ' || * p == '\t'){
                
            }
            else{
                buffer_append(line, p, 1);
            }
        }
        else if(s == 7){
            //bone 1 2
            if(* p == '\r' || * p == '\n'){
                

                
                s = 0;
                if(p[1] =='\n'){
                    p++;
                }
            }
            else{
                buffer_append(line, p, 1);
            }
        }
        else if(s == 10){
            if(* p == '\r' || * p == '\n'){
                s = 0;
                if(p[1] =='\n'){
                    p++;
                }
            }
        }
        p++;
    }
    
    m.length = sizeof(item) * serial_list_count(items);
    if(m.length >0){
        m.data = malloc(m.length);
        memcpy(m.data, items->data, m.length);
    }

    buffer_dealloc(cmd);
    buffer_dealloc(line);
    buffer_dealloc(content);
    serial_list_dealloc(vertexts);
    serial_list_dealloc(normals);
    serial_list_dealloc(txtCoords);
    serial_list_dealloc(items);
    
    return m;
}