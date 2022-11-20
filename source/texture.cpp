#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <SOIL/SOIL.h>

#include "texture.h"

Texture load_texture(const std::string& path) {
	Texture texture;
	glGenTextures(1, &texture.id);
	
	glBindTexture(GL_TEXTURE_2D, texture.id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	u8* image_data = SOIL_load_image(path.c_str(), &texture.width, &texture.height, 0, SOIL_LOAD_RGB);

	printf("texture (%s): %lx, %d, %d, %s\n", path.c_str(), (u64)image_data, texture.width, texture.height, SOIL_last_result());
	if(!image_data) exit(-1);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image_data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}
void set_uniform_texture(u32 location, u32 texture_id, u32 delta) {
	glActiveTexture(GL_TEXTURE0 + delta);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glUniform1i(location, delta);
}
void set_uniform(u32 location , Texture texture, u32 delta) {
	set_uniform_texture(location, texture.id, delta);
}
