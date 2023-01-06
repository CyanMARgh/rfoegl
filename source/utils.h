#pragma once

#include <cstdint>
#include <glm/glm.hpp>

typedef uint8_t u8;
typedef int8_t s8;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

float randf();
vec2 rand_vec2();
vec3 rand_vec3();
vec2 rand_vec2_unit();
vec3 rand_vec3_unit();

vec2 lrot(vec2 a);
float cross(vec2 a, vec2 b);

vec2 lerp(vec2 a, vec2 b, float t);
vec3 lerp(vec3 a, vec3 b, float t);
vec4 lerp(vec4 a, vec4 b, float t);

vec2 normalize(vec2 v, float eps, vec2 u0);
vec3 normalize(vec3 v, float eps, vec3 u0);
vec4 normalize(vec4 v, float eps, vec4 u0);

