#include "image.h"

Image::Image(u32 _width, u32 _height, decltype(GL_RGBA32F) _type) {
	width = _width, height = _height, type = _type;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureStorage2D(id, 1, type, width, height);
}
Image::~Image() {
	glDeleteTextures(1, &id);
}
