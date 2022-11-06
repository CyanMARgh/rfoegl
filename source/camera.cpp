#include "camera.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

void move_camera(Camera& camera, const bool* pressed_keys, float delta_time) {
	float cam_speed = 2.f, rot_speed = 2.0f;
	glm::vec4 direction = {0.f, 0.f, 0.f, 0.f};
	float delta_psi = 0.f, delta_theta = 0.f;
	if(pressed_keys[GLFW_KEY_W]) direction.z -= 1.f;
	if(pressed_keys[GLFW_KEY_S]) direction.z += 1.f;
	if(pressed_keys[GLFW_KEY_A]) direction.x -= 1.f;
	if(pressed_keys[GLFW_KEY_D]) direction.x += 1.f;	
	if(pressed_keys[GLFW_KEY_SPACE]) direction.y += 1.f;	
	if(pressed_keys[GLFW_KEY_LEFT_SHIFT]) direction.y -= 1.f;	
	if(pressed_keys[GLFW_KEY_Q]) delta_psi += 1.f;
	if(pressed_keys[GLFW_KEY_E]) delta_psi -= 1.f;
	if(pressed_keys[GLFW_KEY_R]) delta_theta += 1.f;
	if(pressed_keys[GLFW_KEY_F]) delta_theta -= 1.f;

	camera.rotation = glm::mat4{1.f};
	camera.rotation = glm::rotate(camera.rotation, camera.psi, glm::vec3(0.f, 1.f, 0.f));

	camera.psi += rot_speed * delta_psi * delta_time; 
	camera.theta += rot_speed * delta_theta * delta_time; 

	direction = camera.rotation * direction * cam_speed * delta_time;
	glm::vec3 direction3 = {direction.x, direction.y, direction.z};

	camera.translation = glm::translate(camera.translation, direction3);

	camera.rotation = glm::rotate(camera.rotation, camera.theta, glm::vec3(1.f, 0.f, 0.f));
}