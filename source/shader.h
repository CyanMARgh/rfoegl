#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "texture.h"
#include "utils.h"
#include "image.h"

// std::string read_entire_file(const std::string& path);
u32 get_shader(const std::string& path, int type, u32 first, const std::vector<std::string>& flags);

struct Shader {
	enum Type {
		C, VF, VGF
	} type;
	u32 id;
	std::unordered_map<std::string, u32> fields = {};
	bool is_owner = true;

	// TODO type here seems useless
	Shader(Type type, const std::vector<std::string>& flags);
	~Shader();
	void move(Shader&);
	Shader(Shader&&);	
	Shader& operator=(Shader&&);

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	u32 find(const std::string& field);

	void set_texture(const std::string& field, u32 texture_id, u32 delta = 0);
	void set(const std::string& field, Texture texture, u32 delta = 0);

	void set_image(const std::string& field, u32 texture_id, u32 delta = 0, decltype(GL_RGBA32F) _type = GL_RGBA32F);
	void set(const std::string& field, const Image& image, u32 delta = 0);

	void set(const std::string& field, float value);
	void set(const std::string& field, vec2 value);
	void set(const std::string& field, vec3 value);
	void set(const std::string& field, vec4 value);

	void set(const std::string& field, mat4 value);

	void use();
};