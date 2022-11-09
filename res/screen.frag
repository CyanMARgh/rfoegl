#version 330 core

uniform sampler2D u_tex0, u_tex1;
uniform vec2 u_screen_factor;

in vec2 uv;
out vec4 color;


void main() {
	vec2 uvn = uv;// * u_screen_factor;
	vec4 cm = texture(u_tex0, uvn);
	float dm = texture(u_tex1, uvn).r;
	color = cm;
	gl_FragDepth = dm;
}