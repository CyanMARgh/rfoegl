#include "utils.h"
#include <cmath>

float randf() { return (float)rand() / RAND_MAX; }
vec2 rand_vec2() { return {randf(), randf()}; }
vec3 rand_vec3() { return {randf(), randf(), randf()}; }
vec2 rand_vec2_unit() {
	float phi = randf() * 2.f * M_PI;
	return {cosf(phi), sinf(phi)}; 
}
vec3 rand_vec3_unit() { 
	float phi = randf() * 2.f * M_PI;
	float y = randf() * 2.f - 1.f, r = sqrtf(1.f - y *y);
	return {r * cosf(phi), y, r * sinf(phi)}; 
}

vec2 lrot(vec2 a) { return {-a.y, a.x}; }
float cross(vec2 a, vec2 b) { return a.x * b.y - a.y * b.x; }

vec2 lerp(vec2 a, vec2 b, float t) { return a + t * (b - a); }
vec3 lerp(vec3 a, vec3 b, float t) { return a + t * (b - a); }
vec4 lerp(vec4 a, vec4 b, float t) { return a + t * (b - a); }

vec2 normalize(vec2 v, float eps, vec2 u0) { 
	float l = glm::length(v);
	return l < eps ? u0 : v / l;
}
vec3 normalize(vec3 v, float eps, vec3 u0) { 
	float l = glm::length(v);
	return l < eps ? u0 : v / l;
}
vec4 normalize(vec4 v, float eps, vec4 u0) { 
	float l = glm::length(v);
	return l < eps ? u0 : v / l;
}
