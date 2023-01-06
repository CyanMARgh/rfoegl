#version 430 core

#ifndef DIM
#define DIM 4
#endif

#if DIM == 1
	#define Q r32f
#elif DIM == 2
	#define Q rg32f
#elif DIM == 4
	#define Q rgba32f
#endif

layout(Q, binding = 0) uniform image2D in_tex;
in vec2 uv;
out vec4 o_color;

void main() {
	ivec2 dims = imageSize(in_tex);
	ivec2 p0 = ivec2(vec2(uv.x, 1 - uv.y) * vec2(dims));

#if DIM == 1
	o_color = vec4((imageLoad(in_tex, p0)).xxx, 1);
#elif DIM == 2
	o_color = vec4((imageLoad(in_tex, p0)).xy, 0, 1);
#elif DIM == 4
	o_color = imageLoad(in_tex, p0);
#endif
}

