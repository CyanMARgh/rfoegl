#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>

#include <map>

#include "texture.h"

Texture _load_texture2(const std::string& path, aiTextureType type) {
	Texture_Info texture; texture.type = type;
	glGenTextures(1, &(texture.id));
	int dim;
	u8* image_data = SOIL_load_image(path.c_str(), &(texture.width), &(texture.height), &dim, 0);
	printf("texture (%s): %lx, %d, %d\n", path.c_str(), (u64)image_data, texture.width, texture.height);
	if(image_data) {
		GLenum fmt;
		if(dim == 1) {
			fmt = GL_RED;
			puts("fmt = GL_RED");
		} else if(dim == 3) {
			fmt = GL_RGB;
			puts("fmt = GL_RGB");
		} else if(dim == 4) {
			fmt = GL_RGBA;
			puts("fmt = GL_RGBA");
		} else {
			puts("unknown format\n");
			exit(-1);
		}

		glBindTexture(GL_TEXTURE_2D, texture.id);
		glTexImage2D(GL_TEXTURE_2D, 0, fmt, texture.width, texture.height, 0, fmt, GL_UNSIGNED_BYTE, image_data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, fmt == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, fmt == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SOIL_free_image_data(image_data);
	} else {
		printf("failed to load texture : %s\n", SOIL_last_result());
		exit(-1);
	}
	return {texture, path};
}
// TODO {Texture_Info, u32} -> Texture_Info = {Textre_Base, u32}
std::map<std::string, std::pair<Texture_Info, u32>> loaded_textures = {};
Texture load_texture(const std::string& path, aiTextureType type) {
	if(auto F = loaded_textures.find(path); F != loaded_textures.end()) {
		F->second.second++;
		return {F->second.first, path};
	} else {
		Texture texture = _load_texture2(path, type);
		loaded_textures[path] = {texture.info, 1};
		return texture;
	}
}

void set_uniform_texture(u32 location, u32 texture_id, u32 delta) {
	glActiveTexture(GL_TEXTURE0 + delta);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glUniform1i(location, delta);
}
void set_uniform(u32 location , Texture texture, u32 delta) {
	set_uniform_texture(location, texture.info.id, delta);
}
void clear(Texture texture) {
	if(auto F = loaded_textures.find(texture.path); F != loaded_textures.end()) {
		if(!--(F->second.second)) {
			glDeleteTextures(1, &(F->second.first.id));			
			loaded_textures.erase(F);
			printf("tex -\n");
		}
	} else {
		printf("trying to delete non-existing texture\n");
		exit(-1);
	}
}
