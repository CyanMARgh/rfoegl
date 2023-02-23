#version 430 core

struct Node {
	vec4 color;
	uint prev;
	float depth;
};

layout (early_fragment_tests) in;

layout (binding = 0) uniform sampler2D u_tex_diff;
layout (binding = 1) uniform sampler2D u_tex_spec;
layout (binding = 2) uniform sampler2D u_tex_norm;

layout (r32ui, binding = 0) uniform uimage2D u_heads;
layout (binding = 0, std430) buffer u_lists {
	Node nodes[];
};
layout (binding = 0) uniform atomic_uint pixel_counter;
const uint max_nodes = 1200 * 800 * 5;

in struct Vert_Out {
	vec3 pos;
	vec2 uv;
	mat3 N;
} vert_out;

out vec4 o_color; //TODO remove?

void main() {
	vec2 uv = vert_out.uv;
#ifdef FLIPUV
	uv.y = 1 - uv.y;
#endif
	vec4 color = texture(u_tex_diff, uv);
	ivec2 coord = ivec2(gl_FragCoord.xy);

	uint node_id = atomicCounterIncrement(pixel_counter);
	if(node_id < max_nodes) {
		uint prev = imageAtomicExchange(u_heads, coord, node_id);
		nodes[node_id] = Node(color, prev, gl_FragCoord.z);
	}
}


