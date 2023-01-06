#version 430 core

uniform sampler2D u_screen;

#ifdef FLIPUV
vec2 fixuv(vec2 p) {
	return vec2(p.x, 1 - p.y);
}
#else
vec2 fixuv(vec2 p) {
	return p;
}
#endif

in vec2 uv;
out vec4 o_color;



void main() {
	o_color = texture(u_screen, fixuv(uv));
}