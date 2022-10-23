#version 330 core

in vec4 vertex_color;
in vec2 tex_uv;
out vec4 color;  

uniform sampler2D u_texture;

void main() {
    //color = (vertex_color + texture(u_texture, tex_uv)) * .5;
    color = texture(u_texture, vec2(tex_uv.x, 1 - tex_uv.y));
}
