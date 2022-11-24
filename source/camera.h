#pragma once

#include <glm/glm.hpp>

#include "utils.h"
#include "window.h"

struct Camera {
	float psi = 0.f, theta = 0.f;

	glm::mat4 translation{1.f};
	glm::mat4 rotation{1.f};
};
void move_camera(Camera* camera, const bool* pressed_keys, float delta_time);
vec3 get_position(Camera camera);
glm::mat4 get_transform(const Camera* camera, const Window* window);