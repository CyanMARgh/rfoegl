#include "framebuffer.h"
#include <cstdio>

Frame_Buffer::Frame_Buffer(u32 _width, u32 _height) {
	width = _width, height = _height;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		//TODO change to parameters (types of textures) or make different framebuffer t
		//color texture
		glGenTextures(1, &color_texture_id);
		glBindTexture(GL_TEXTURE_2D, color_texture_id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

			// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture_id, 0);

		glGenTextures(1, &depth_texture_id);
		glBindTexture(GL_TEXTURE_2D, depth_texture_id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_id, 0);

		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			printf("ERROR: Framebuffer is not complete!");
		}
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_id, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);  	
}
Frame_Buffer::~Frame_Buffer() {
	glDeleteTextures(1, &depth_texture_id);
	glDeleteTextures(1, &color_texture_id);
	glDeleteRenderbuffers(1, &RBO);
	glDeleteFramebuffers(1, &FBO);
}

void set_buffer(const Frame_Buffer* frame_buffer) {
	glViewport(0, 0, frame_buffer->width, frame_buffer->height);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->FBO);	
}
void set_window_buffer(const Window* window) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window->width, window->height);
}