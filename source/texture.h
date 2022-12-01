#pragma once

#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "utils.h"

struct Texture {
	u32 id;
	int width, height;
	aiTextureType type;
};
Texture load_texture(const std::string& path, aiTextureType type = aiTextureType_DIFFUSE);
void set_uniform_texture(u32 location, u32 texture_id, u32 delta = 0);
void set_uniform(u32 location , Texture texture, u32 delta = 0);