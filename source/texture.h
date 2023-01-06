#pragma once

#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "utils.h"

struct Texture_Info {
	u32 id, owners;
	int width, height;
	aiTextureType type;
};
struct Texture {
	Texture_Info info;
	std::string path;
};

Texture _load_texture2(const std::string& path, aiTextureType type = aiTextureType_DIFFUSE);

Texture load_texture(const std::string& path, aiTextureType type = aiTextureType_DIFFUSE);
void set_uniform_texture(u32 location, u32 texture_id, u32 delta = 0);
void set_uniform(u32 location , Texture texture, u32 delta = 0);
void clear(Texture texture);

void clear_texture_manager();