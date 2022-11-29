#pragma once

#include <string>

#include "utils.h"

struct Texture {
	u32 id;
	int width, height;
};
Texture load_texture(const std::string& path, bool gamma_correction);
void set_uniform_texture(u32 location, u32 texture_id, u32 delta = 0);
void set_uniform(u32 location , Texture texture, u32 delta = 0);