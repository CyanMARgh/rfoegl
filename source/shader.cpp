#define GLEW_STATIC
#include <GL/glew.h>

#include <cstdio>
#include <fstream>
#include <cstdint>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

// std::string read_entire_file(const std::string& path) {
// 	std::ifstream file(path);
// 	if(!file) {
// 		printf("failed to open file: %s\n", path.c_str());
// 		exit(-1);
// 	}
// 	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
// 	return content;
// }
u32 get_shader(const std::string& path, decltype(GL_VERTEX_SHADER) type, u32 first, const std::vector<std::string>& flags) {
	u32 shader = glCreateShader(type);
	// printf("+shader %d\n", shader);

	std::ifstream infile(path);
	if(infile.fail()) {
		printf("failed to open file: %s\n", path.c_str());
		exit(-1);
	}
	std::stringstream source_sb;
	std::string line_buf;

	std::getline(infile, line_buf);
	source_sb << line_buf << '\n';
	for(u32 i = first, n = flags.size(); i < n; i++) {
		source_sb << "#define " << flags[i] << '\n';
	}
	while (std::getline(infile, line_buf)) {
		source_sb << line_buf << "\n";
	}

	std::string source = source_sb.str();
	const char* source_ptr = source.c_str();

	glShaderSource(shader, 1, &source_ptr, NULL);
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


Shader::Shader(Type type, const std::vector<std::string>& flags) : type(type) {
	u32 spc = type == C ? 1 : type == VF ? 2 : type == VGF ? 3 : 0;
	int types[3];
	u32 pids[3];
	u32 n = flags.size();
	if(n < spc) {
		printf("to few flags (%d / %d)\n", n, spc);
		exit(-1);
	}
	if(type == C) {
		types[0] = GL_COMPUTE_SHADER;
	} else if(type == VF) {
		types[0] = GL_VERTEX_SHADER, types[1] = GL_FRAGMENT_SHADER;		
	} else if(type == VGF) {
		types[0] = GL_VERTEX_SHADER, types[1] = GL_GEOMETRY_SHADER, types[2] = GL_FRAGMENT_SHADER;		
	} else {
		printf("unknown shader program type : %d\n", type);
		exit(-1);
	}
	id = glCreateProgram();
	// printf("+prog %d\n", id);
	for(u32 i = 0; i < spc; i++) {
		pids[i] = get_shader(flags[i], types[i], spc, flags);
		glAttachShader(id, pids[i]);
	}
	glLinkProgram(id);
	for(u32 i = 0; i < spc; i++) {
		glDeleteShader(pids[i]);
		// printf("-shader %d\n", pids[i]);
	}
	puts("shader linked succesfully");
}
Shader::~Shader() {
	if(is_owner) {
		// printf("-prog %d\n", id);
		glDeleteProgram(id);
	}
}
void Shader::move(Shader& other) {
	is_owner = other.is_owner;
	other.is_owner = false;

	type = other.type;
	id = other.id;
	fields = std::move(other.fields);
}
Shader::Shader(Shader&& other) {
	move(other);
}
Shader& Shader::operator=(Shader&& other) {
	if(&other != this) {
		move(other);
	}
	return *this;
}


u32 Shader::find(const std::string& field) {
	auto F = fields.find(field);
	if(F == fields.end()) {
		u32 field_id = glGetUniformLocation(id, field.c_str());
		return fields[field] = field_id;
		// printf("field doesn't exist: %s\n", field.c_str());
		// exit(-1);
	} else {
		return F->second;
	}
}
void Shader::use() {
	glUseProgram(id);
}

// void Shader::set_texture(const std::string& field, u32 texture_id, u32 delta) {
// 	glActiveTexture(GL_TEXTURE0 + delta);
// 	glBindTexture(GL_TEXTURE_2D, texture_id);
// 	// glUniform1i(find(field), delta);
// }
// void Shader::set(const std::string& field, Texture texture, u32 delta) {
// 	set_texture(field, texture.info.id, delta);
// }
void Shader::set_image(const std::string& field, u32 texture_id, u32 delta, decltype(GL_RGBA32F) type) {
	//TODO binding -> image method
	glBindImageTexture(delta, texture_id, 0, GL_FALSE, 0, GL_WRITE_ONLY, type);
	// glUniform1i(find(field), delta);
}
void Shader::set(const std::string& field, const Image& image, u32 delta) {
	set_image(field, image.id, delta, image.type);
}

void Shader::set(const std::string& field, float value) {
	glUniform1f(find(field), value);
}
void Shader::set(const std::string& field, vec2 value) {
	glUniform2f(find(field), value.x, value.y);
}
void Shader::set(const std::string& field, vec3 value) {
	glUniform3f(find(field), value.x, value.y, value.z);
}
void Shader::set(const std::string& field, vec4 value) {
	glUniform4f(find(field), value.x, value.y, value.z, value.w);
}
void Shader::set(const std::string& field, mat4 value) {
	glUniformMatrix4fv(find(field), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(const std::string& field, u32 value) {
	glUniform1ui(find(field), value);
}
void Shader::set(const std::string& field, s32 value) {
	glUniform1i(find(field), value);
}
