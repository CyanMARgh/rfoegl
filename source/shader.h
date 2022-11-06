#pragma once

#include <string>
#include "utils.h"

std::string read_entire_file(const std::string& path);
u32 get_shader(const std::string& path, int type);
u32 get_shader_program_VF(const std::string& path_vert, const std::string& path_frag);
u32 get_shader_program_VGF(const std::string& path_vert, const std::string& path_geom,  const std::string& path_frag);