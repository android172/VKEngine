#version 450

layout(binding=0)uniform uniform_buffer_object{
    mat4 model;
    mat4 view;
    mat4 project;
}ubo;

layout(location=0)in vec2 in_position;
layout(location=1)in vec3 in_color;

layout(location=0)out vec3 frag_color;

void main(){
    gl_Position=ubo.project*ubo.view*ubo.model*vec4(in_position,0.,1.);
    frag_color=in_color;
}