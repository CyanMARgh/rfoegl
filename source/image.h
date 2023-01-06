#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include "utils.h"

struct Image {
	u32 id;
	u32 width, height;
	decltype(GL_RGBA32F) type;
	Image(u32 _width, u32 _height, decltype(GL_RGBA32F) _type = GL_RGBA32F);
	Image(const Image& image) = delete;
	Image& operator=(const Image& image) = delete;
	~Image();
};
