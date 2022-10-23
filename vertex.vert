#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 _tex_uv;

uniform float u_time;

out vec4 vertex_color;
out vec2 tex_uv;

void main() {
    gl_Position = vec4(position, 1.);
    vertex_color = vec4((sin(u_time * 3.) * .5 + .5) * color, 1.);
    tex_uv = _tex_uv;
}