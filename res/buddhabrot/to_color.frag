#version 430 core

layout(r32ui, binding = 0) uniform uimage2D u_brbuf;

// in struct Vert_Out {
// 	vec3 pos;
// 	vec2 uv;

// 	mat3 N;
// } vert_out;

out vec4 o_color;
uniform uint u_time;

vec3 afmhot(float t) {
    const vec3 c0 = vec3(-0.020390,0.009557,0.018508);
    const vec3 c1 = vec3(3.108226,-0.106297,-1.105891);
    const vec3 c2 = vec3(-14.539061,-2.943057,14.548595);
    const vec3 c3 = vec3(71.394557,22.644423,-71.418400);
    const vec3 c4 = vec3(-152.022488,-31.024563,152.048692);
    const vec3 c5 = vec3(139.593599,12.411251,-139.604042);
    const vec3 c6 = vec3(-46.532952,-0.000874,46.532928);
    return c0+t*(c1+t*(c2+t*(c3+t*(c4+t*(c5+t*c6)))));
}

void main() {
	uint ibr = imageLoad(u_brbuf, ivec2(gl_FragCoord.xy)).r;
	float br = float(ibr) / (u_time * 5);
	float t = log(1 + br * .7);
	o_color = vec4(afmhot(clamp(t, 0, 1)), 1);

	// vec2 uv = vert_out.uv;
	// o_color = vec4(uv, 0, 1);
	// o_color = vec4(1, 0, 0, 1);
}