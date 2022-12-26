#pragma once

#include <string>
#include "utils.h"

//TODO find id by name, use() method
struct Shader {
	u32 id;
};

std::string read_entire_file(const std::string& path);
u32 get_shader(const std::string& path, int type);
Shader get_shader_program_C(const std::string& path_comp);
Shader get_shader_program_VF(const std::string& path_vert, const std::string& path_frag);
Shader get_shader_program_VGF(const std::string& path_vert, const std::string& path_geom,  const std::string& path_frag);
void clear(Shader shader);
