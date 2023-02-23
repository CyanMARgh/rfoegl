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

vec2 hash22(vec2 p) {
    p = p * mat2(127.1, 311.7, 269.5 ,183.3);
	p = -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
	return sin(p * 6.283);
}
float perlin_level(vec2 p) {
	vec2 pi = floor(p);
    vec2 pf = p - pi;
    vec2 w = pf;
    
    vec2
        d00 = hash22(pi + vec2(0., 0.)),
        d01 = hash22(pi + vec2(0., 1.)),
        d10 = hash22(pi + vec2(1., 0.)),
        d11 = hash22(pi + vec2(1., 1.));

    float
        f00 = dot(d00, pf - vec2(0., 0.)),
        f01 = dot(d01, pf - vec2(0., 1.)),
        f10 = dot(d10, pf - vec2(1., 0.)),
        f11 = dot(d11, pf - vec2(1., 1.));
    
    float
        m00 = f00 * (1. - w.x) * (1. - w.y),
        m01 = f01 * (1. - w.x) *        w.y,
        m10 = f10 *        w.x * (1. - w.y),
        m11 = f11 *        w.x *        w.y;
    
    return m00 + m01 + m10 + m11;   
}
vec3 perlin_level_full(vec2 p) {
	vec2 pi = floor(p);
    vec2 pf = p - pi;
    vec2 w = pf * pf * (3. - 2. * pf);
        
    vec2
        d00 = hash22(pi + vec2(0., 0.)),
        d01 = hash22(pi + vec2(0., 1.)),
        d10 = hash22(pi + vec2(1., 0.)),
        d11 = hash22(pi + vec2(1., 1.));

    float
        f00 = dot(d00, pf - vec2(0., 0.)), // * (1-x) * (1-y)
        f01 = dot(d01, pf - vec2(0., 1.)), // * x * (1-y)
        f10 = dot(d10, pf - vec2(1., 0.)), // * (1 -x) * y
        f11 = dot(d11, pf - vec2(1., 1.)); // * x * y
    
    vec2 gw = 6. * (pf - pf * pf); 
    vec2
        g00 = d00 * (1. - w.x) * (1. - w.y) + f00 * vec2((w.y - 1.), (w.x - 1.)) * gw,
        g01 = d01 * (1. - w.x) * w.y        + f01 * vec2(-w.y, (1. - w.x)) * gw,
        g10 = d10 * w.x * (1. - w.y)        + f10 * vec2((1. - w.y),  -w.x) * gw,
        g11 = d11 * w.x * w.y               + f11 * vec2(w.y, w.x) * gw;
        
    float h = 
        f00 * (1. - w.x) * (1. - w.y) +
        f01 * (1. - w.x) * w.y +
        f10 * w.x * (1. - w.y) + 
        f11 * w.x * w.y;
    vec2 gh = g00 + g01 + g10 + g11;
    return vec3(h, gh); 
}
float perlin(vec2 p) {
	float a = 1., r = 0., s=0.;    
    for(int i = 0; i < 8; i++) {
      r += a * perlin_level(p);
      s += a; p *= 2.; a *= .5;
    }
    return r / s;
}
vec3 perlin_full(vec2 p, int I) {
    vec3 r = vec3(0.);
    float a = 1., s = 0.;
    for(int i = 0; i < I; i++) {
      r += perlin_level_full(p) * vec3(a, 1., 1.);
      s += a; p *= 2.; a *= .5;
    }
    r /= s;    
    return vec3(r.x * .5 + .5, .5 * r.yz);
}
vec4 color_paper(vec2 frag_coord) {
	vec2 uv = frag_coord / vec2(1200, 800) * mat2(1., 0., 0., 1.) + 10.;    

	uv *= 3.;    
	vec2 offset = vec2(.2);
	vec3 p = perlin_full(uv, 7);
	vec3 color_paper = vec3(mix(vec3(0.0, 0.0, -.3), vec3(1.), dot(p.xz, offset) * .1 + .9));

	return vec4(color_paper, 1);
}

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
	vec4 color = color_paper(gl_FragCoord.xy);// vec4(1, 1, 1, 1);

	for (uint i = 0; i < count; i++) {    	
		color = mix(color, frags[i].color, frags[i].color.a);
	}
	o_color = vec4(color.rgb, 1);
}


