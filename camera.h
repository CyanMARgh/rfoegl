#include <glm/glm.hpp>

struct Camera {
	glm::mat4 translation{1.f};
	glm::mat4 rotation{1.f};
};
void move_camera(Camera& camera, const bool* pressed_keys, float delta_time);