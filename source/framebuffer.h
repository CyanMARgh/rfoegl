#pragma once

#include "window.h"

struct Frame_Buffer {
	u32 FBO, RBO, color_texture_id, depth_texture_id, width, height;
	Frame_Buffer(u32 _width, u32 _height);
	~Frame_Buffer();
};
void set_buffer(const Frame_Buffer* frame_buffer);
void set_window_buffer(const Window* window);