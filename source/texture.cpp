#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>

#include <map>

#include "texture.h"

Texture _load_texture2(const std::string& path, aiTextureType type) {
	Texture_Info texture; texture.type = type;
	
	printf("+ texture: %d\n", texture.id);
	int dim;
	u8* image_data = SOIL_load_image(path.c_str(), &(texture.width), &(texture.height), &dim, 4);
	printf("texture (%s): %lx, %d, %d\n", path.c_str(), (u64)image_data, texture.width, texture.height);
	if(!image_data) {
		printf("failed to load texture : %s\n", SOIL_last_result());
		exit(-1);
	}
	GLenum fmt;
	if(dim == 1) {
		fmt = GL_RED;
		puts("fmt = GL_RED");
	} else if(dim == 3) {
		fmt = GL_RGBA;
		puts("fmt = GL_RGB");
	} else if(dim == 4) {
		fmt = GL_RGBA;
		puts("fmt = GL_RGBA");
	} else {
		puts("unknown format\n");
		exit(-1);
	}

	glGenTextures(1, &(texture.id));
	glBindTexture(GL_TEXTURE_2D, texture.id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, fmt == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, fmt == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// glBindImageTexture(0, texture.id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	// u32 output_tex;
	// glGenTextures(1, &output_tex); DEFER(glDeleteTextures(1, &output_tex));
	// glBindTexture(GL_TEXTURE_2D, output_tex);

	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// glTextureStorage2D(output_tex, 1, GL_RGBA32F, 1200, 800);
	// glBindImageTexture(0, output_tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	SOIL_free_image_data(image_data);
	return {texture, path};
}
// TODO {Texture_Info, u32} -> Texture_Info = {Textre_Base, u32}
std::map<std::string, std::pair<Texture_Info, u32>> loaded_textures = {};
Texture load_texture(const std::string& path, aiTextureType type) {
	if(auto F = loaded_textures.find(path); F != loaded_textures.end()) {
		F->second.second++;
		// printf("found: %d\n", F->second.first.id);
		return {F->second.first, path};
	} else {
		Texture texture = _load_texture2(path, type);
		loaded_textures[path] = {texture.info, 1};
		// printf("found: %d\n", texture.info.id);
		return texture;
	}
}

void bind_texture(u32 texture_id, u32 delta) {
	glActiveTexture(GL_TEXTURE0 + delta);
	glBindTexture(GL_TEXTURE_2D, texture_id);
}
void bind(Texture texture, u32 delta) {
	bind_texture(texture.info.id, delta);
}
void clear(Texture texture) {
	printf("clear texture start!\n");
	if(auto F = loaded_textures.find(texture.path); F != loaded_textures.end()) {
		if(!--(F->second.second)) {
			printf("- texture: %d\n", F->second.first.id);
			glDeleteTextures(1, &(F->second.first.id));			
			loaded_textures.erase(F);
			printf("tex -\n");
		}
	} else {
		printf("trying to delete non-existing texture\n");
		exit(-1);
	}
}

void clear_texture_manager() {
	for(auto [path, tex] : loaded_textures) {
		glDeleteTextures(1, &(tex.first.id));					
	}
	loaded_textures = {};
}