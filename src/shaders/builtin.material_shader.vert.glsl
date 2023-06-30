#version 450

layout(set=0,binding=0)uniform global_uniform_buffer{
    mat4 projection;
    mat4 view;
    vec4 ambient_color;
    vec3 view_position;
    uint mode;
}UBO;

layout(push_constant)uniform push_constants{
    
    // Only guaranteed a total of 128 bytes.
    mat4 model;// 64 bytes
}PC;

layout(location=0)in vec3 in_position;
layout(location=1)in vec3 in_normal;
layout(location=2)in vec4 in_tangent;
layout(location=3)in vec4 in_color;
layout(location=4)in vec2 in_texture_coordinate;

layout(location=0)out uint out_mode;

// Data transfer object
layout(location=1)out struct data_transfer_object{
    vec4 ambient_color;
    vec3 surface_normal;
    vec4 surface_tangent;
    vec2 texture_coordinate;
    vec3 view_position;
    vec3 frag_position;
    vec4 color;
}DTO;

void main(){
    gl_Position=UBO.projection*UBO.view*PC.model*vec4(in_position,1.);
    
    out_mode=UBO.mode;
    
    mat3 model_m3=mat3(PC.model);
    
    DTO.ambient_color=UBO.ambient_color;
    DTO.surface_normal=normalize(model_m3*in_normal);
    DTO.surface_tangent=vec4(normalize(model_m3*in_tangent.xyz),in_tangent.w);
    DTO.texture_coordinate=in_texture_coordinate;
    DTO.view_position=UBO.view_position;
    DTO.frag_position=vec3(PC.model*vec4(in_position,1.));
    DTO.color=in_color;
}