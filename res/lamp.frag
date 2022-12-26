#version 420 core

layout(binding = 0) uniform atomic_uint pixel_counter;

uniform vec3 u_color;
out vec4 o_color;

void main() {
	uint x = atomicCounterIncrement(pixel_counter);
	o_color = vec4(float(x % 16u) / 16 * u_color, 1);
}