#define GLEW_STATIC
#include <GL/glew.h>

#include <cstdio>
#include <fstream>
#include <cstdint>

#include "shader.h"

std::string read_entire_file(const std::string& path) {
	std::ifstream file(path);
	if(!file) {
		printf("failed to open file: %s\n", path.c_str());
		exit(-1);
	}
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	return content;
}
u32 get_shader(const std::string& path, decltype(GL_VERTEX_SHADER) type) {
	u32 shader = glCreateShader(type);
	auto _shader_source = read_entire_file(path);
	auto shader_source = _shader_source.c_str();
	glShaderSource(shader, 1, &shader_source, NULL);
	glCompileShader(shader);
	s32 succes;
	s8 info_log[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &succes);
	if(!succes) {
		glGetShaderInfoLog(shader, 512, NULL, (GLchar*)info_log);
		printf("failed to compile shader (path: %s): %s\n", path.c_str(), info_log);
		exit(-1);
	}
	printf("shader compiled succesfully (path: %s)\n", path.c_str());
	return shader;
}
u32 get_shader_program_VF(const std::string& path_vert, const std::string& path_frag) {
	u32 vertex_shader = get_shader(path_vert, GL_VERTEX_SHADER);
	u32 fragment_shader = get_shader(path_frag, GL_FRAGMENT_SHADER);
	u32 shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	puts("shader linked succesfully");
	return shader_program;
}
u32 get_shader_program_VGF(const std::string& path_vert, const std::string& path_geom,  const std::string& path_frag) {
	u32 vertex_shader = get_shader(path_vert, GL_VERTEX_SHADER);
	u32 geometry_shader = get_shader(path_geom, GL_GEOMETRY_SHADER);
	u32 fragment_shader = get_shader(path_frag, GL_FRAGMENT_SHADER);
	u32 shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, geometry_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glDeleteShader(vertex_shader);
	glDeleteShader(geometry_shader);
	glDeleteShader(fragment_shader);
	puts("shader linked succesfully");
	return shader_program;
}