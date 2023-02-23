#version 430 core

struct Node {
	vec4 color;
	uint prev;
	float depth;
};

layout (early_fragment_tests) in;
layout (r32ui, binding = 0) uniform uimage2D u_heads;
layout (binding = 0, std430) buffer u_lists {
	Node nodes[];
};
layout (binding = 0) uniform atomic_uint pixel_counter;
const uint max_nodes = 1200 * 800 * 15;

in struct Geom_Out {
	vec2 uv;
	vec3 color;
	float t, r;
} geom_out;

out vec4 o_color; 

const int M1 = 2, M2 = 2;
vec2 hash22(vec2 p) {
    p = p * mat2(127.1, 311.7, 269.5 ,183.3);
	p = -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
	return sin(p * 6.283 + geom_out.t);
}
float perlin_level(vec2 p) {
	vec2 pi = floor(p);
    vec2 pf = p - pi;
    vec2 w = pf * pf * (3. - 2. * pf);
    
    float f00 = dot(hash22(pi + vec2(0., 0.)),pf - vec2(0., 0.));
    float f01 = dot(hash22(pi + vec2(0., 1.)),pf - vec2(0., 1.));
    float f10 = dot(hash22(pi + vec2(1., 0.)),pf - vec2(1., 0.));
    float f11 = dot(hash22(pi + vec2(1., 1.)),pf - vec2(1., 1.));
        
    return mix(mix(f00, f10, w.x), mix(f01, f11, w.x), w.y);   
}
float perlin(vec2 p) {
	float a = 1., r = 0., s=0.;    
    for(int i = 0; i < M1; i++) {
      r += a * perlin_level(p);
      s += a; p *= 2.; a *= .5;
    }
    return r / s;
}

vec2 perlin_22(vec2 p) {
    return vec2(perlin(p), perlin(p + 1000.));
}
float mix_paint(float back, float front) {
    return front * front + (1. - front) * back;
}

float paint_layer(float r0, vec2 uv, float dr2, float seed) {
    float r = r0 + perlin(uv * 10. + seed) * dr2;    
    float brightness = r > 0. ?
        mix(0., 1., exp(r * -20.)):
        mix(0.8, 1., exp(r * 5.));
    
    return clamp(brightness, 0., 1.);
}
float paint_effect(float r0, vec2 uv, float dr1, float dr2) {
    r0 += perlin(uv * 5. + 10.) * dr1;
    
    float brightness = 0.;
    for(int i = 0; i < M2; i++) {
        float fi = float(i);
        brightness = mix_paint(paint_layer(r0, uv, dr2, fi) - .1 * fi, brightness);
    }
    
    return brightness * pow(perlin(uv * 5. + 10.) * .5 + .5, .03);
}
vec3 hsv2rgb(vec3 c) {
	vec4 K = vec4(1., 2. / 3., 1. / 3., 3.);
	return c.z * mix(K.xxx, clamp(abs(fract(c.x + K.xyz) * 6. - K.w) - K.x, 0, 1), c.y);
}
vec3 rgb2hsv(vec3 c) {
	float
		cMax = max(max(c.r, c.g), c.b),
		cMin = min(min(c.r, c.g), c.b),
		delta = cMax - cMin;
	vec3 hsv = vec3(0, 0, cMax);
	if(cMax > cMin){
		hsv.y = delta / cMax;
		if(c.r == cMax){
			hsv.x = (c.g - c.b) / delta;
		}else if(c.g == cMax){
			hsv.x = 2 + (c.b - c.r) / delta;
		}else{
			hsv.x = 4 + (c.r - c.g) / delta;
		}
		hsv.x = fract(hsv.x / 6);
	}
	return hsv;
}
vec4 get_color(vec3 base, vec2 uv) {
	float r = length(uv) - (.5 * geom_out.r); 
	float col = paint_effect(r, uv, .1, .1);
	vec3 hsv = rgb2hsv(base);
	hsv.g = pow(hsv.g, .8);
	hsv.b = smoothstep(0, 1, hsv.b);
	return vec4(hsv2rgb(hsv), col * .5);
}
vec4 get_color_2(vec3 base, vec2 uv) {
	float r = length(uv) - .7 * geom_out.r;
	return vec4(base, r < 0 ? 1 : 0);
}

void main() {
	vec2 uv = geom_out.uv;
	// uvc.y = 1 - uvc.y;
	// vec3 color = texture(u_tex_diff, uvc).rgb;
	vec4 color = get_color(geom_out.color, geom_out.uv);
	if(color.a < 0.01) return;

	ivec2 coord = ivec2(gl_FragCoord.xy);
	uint node_id = atomicCounterIncrement(pixel_counter);
	if(node_id < max_nodes) {
		uint prev = imageAtomicExchange(u_heads, coord, node_id);
		nodes[node_id] = Node(color, prev, gl_FragCoord.z);
	}
}
