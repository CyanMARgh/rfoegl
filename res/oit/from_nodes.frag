#version 430 core

layout (r32ui, binding = 0) uniform uimage2D u_heads;
struct Node {
	vec4 color;
	uint prev;
	float depth;
};
layout (std430, binding = 0) buffer u_lists {
	Node nodes[];
};

out vec4 o_color;

void main() {
	const uint MAX_FRAGMENTS = 75;
	Node frags[MAX_FRAGMENTS];
	uint count = 0, n = imageLoad(u_heads, ivec2(gl_FragCoord.xy)).r;

    while (n != 0xFFFFFFFF && count < MAX_FRAGMENTS) {
		frags[count] = nodes[n];
		n = frags[count].prev;
        count++;
    }

    for (uint i = 1; i < count; i++) {
		Node to_insert = frags[i];
		uint j = i;

		while(j > 0 && to_insert.depth > frags[j-1].depth) {
			frags[j] = frags[j-1];
			j--;
		}
		frags[j] = to_insert;
    }
	vec4 color = vec4(0, 1, 0, 1);

	for (uint i = 0; i < count; i++) {    	
		color = mix(color, frags[i].color, frags[i].color.a);
	}
	o_color = vec4(color.rgb, 1);
}


