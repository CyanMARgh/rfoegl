#version 330 core
layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 12) out;

uniform int u_point_count;
uniform float u_r = .1;
uniform mat4 u_transform;

in struct Vert_Out {
	vec3 norm;
	float time;
	int id;
} vert_out[];
out struct Geom_Out {
	vec2 uv;
} geom_out;

struct Point {
	vec2 uv;
	vec3 P;
};

const vec3 u_screen_scale = vec3(1.5, 1., 1.);

void add_point(Point point) {
	geom_out.uv = vec2(mix(vert_out[1].time, vert_out[2].time, point.uv.x), point.uv.y);
	gl_Position = u_transform * vec4(point.P, 1);
	EmitVertex();	
}
void main() {
	vec3
		A = gl_in[0].gl_Position.xyz, nA = vert_out[0].norm,
		B = gl_in[1].gl_Position.xyz, nB = vert_out[1].norm,
		C = gl_in[2].gl_Position.xyz, nC = vert_out[2].norm,
		D = gl_in[3].gl_Position.xyz, nD = vert_out[3].norm;
	if(vert_out[3].id == u_point_count && vert_out[3].time < -.1) { D = 2 * C - B, nD = nC; }
	if(vert_out[0].id == 0 && vert_out[0].time < -.1) { A = 2 * B - C, nA = nB; }

	vec3
		nAB = normalize(nA + nB), AB = B - A,
		nBC = normalize(nB + nC), BC = C - B,
		nCD = normalize(nC + nD), CD = D - C;
	vec3
		dB = normalize(cross(AB + BC, nB)),
		dC = normalize(cross(BC + CD, nC)),
		dBi = normalize(cross(BC, nB)),
		dCi = normalize(cross(BC, nC));
	// vec3
	// 	nAB = normalize(nA + nB), AB = B - A, dAB = normalize(cross(AB, nAB)),
	// 	nBC = normalize(nB + nC), BC = C - B, dBC = normalize(cross(BC, nBC)),
	// 	nCD = normalize(nC + nD), CD = D - C, dCD = normalize(cross(CD, nCD));
	// vec3
	// 	dB = normalize(dAB + dBC),
	// 	dC = normalize(dBC + dCD);
	// vec3 
	// 	dBi = dBC,
	// 	dCi = dBC;

	bool lb = dot(dB, BC) > 0, rb = dot(dC, BC) < 0;
	float kB = u_r / length(cross(normalize(BC), dB));
	float kC = u_r / length(cross(normalize(BC), dC));

	vec3 P0, P1, P2 = C, P3 = B, P4, P5, P6, P7;
	if(lb) {
		P1 = B + dB * kB,
		P5 = B - dBi * u_r;
	} else {
		P1 = B + dBi * u_r,
		P5 = B - dB * kB;
	}
	if(rb) {
		P0 = C + dC * kC,
		P4 = C - dCi * u_r;
	} else {
		P0 = C + dCi * u_r,
		P4 = C - dC * kC;
	}
	P6 = B + dB * u_r * (lb ? -1 : 1);
	P7 = C + dC * u_r * (rb ? -1 : 1);

	float
		l54 = length(P5 - P4),
		l10 = length(P0 - P1),
		l6  = length(dB - dBi) * u_r,
		l7  = length(dC - dCi) * u_r;
	float t1, t2;
	
	if(lb && rb) {
		t1 = l6 / (l6 + l54 + l7);
		t2 = (l6 + l54) / (l6 + l54 + l7);
	} else if(lb) {
		t1 = l6 / (l6 + l54);
		t2 = l10 / (l10 + l7);
	} else if(rb) {
		t1 = l6 / (l6 + l10);
		t2 = l54 / (l54 + l7);		
	} else {
		t1 = l6 / (l6 + l10 + l7);
		t2 = (l6 + l10) / (l6 + l10 + l7);
	}
	
	Point
		_P0 = Point(vec2(rb ? 1 : t2,        0.0), P0),
		_P1 = Point(vec2(lb ? 0 : t1,        0.0), P1),
		_P2 = Point(vec2(        1.0,        0.5), P2),
		_P3 = Point(vec2(        0.0,        0.5), P3),
		_P4 = Point(vec2(rb ? t2 : 1,        1.0), P4),
		_P5 = Point(vec2(lb ? t1 : 0,        1.0), P5),
		_P6 = Point(vec2(          0, lb ? 1 : 0), P6),
		_P7 = Point(vec2(          1, rb ? 1 : 0), P7);


	// Point
	// 	_P0 = Point(vec2(1,        0.0), P0),
	// 	_P1 = Point(vec2(0,        0.0), P1),
	// 	_P2 = Point(vec2(1,        0.5), P2),
	// 	_P3 = Point(vec2(0,        0.5), P3),
	// 	_P4 = Point(vec2(1,        1.0), P4),
	// 	_P5 = Point(vec2(0,        1.0), P5),
	// 	_P6 = Point(vec2(0, lb ? 1 : 0), P6),
	// 	_P7 = Point(vec2(1, rb ? 1 : 0), P7);

	//V2
	// add_point(_P5), add_point(_P4), add_point(_P6), add_point(_P7), add_point(_P1), add_point(_P0);
	// EndPrimitive();

	//V1
	add_point(_P6), add_point(_P5), add_point(_P1);
	EndPrimitive();
	add_point(_P0), add_point(_P4), add_point(_P7);
	EndPrimitive();
	add_point(_P1), add_point(_P5), add_point(_P0), add_point(_P4);
	EndPrimitive();

	//V0
	// add_point(_P0), add_point(_P1), add_point(_P2), add_point(_P3), add_point(_P4), add_point(_P5);
	// EndPrimitive();
	// add_point(_P3), add_point(lb ? _P6 : _P1), add_point(lb ? _P5 : _P6);
	// EndPrimitive();
	// add_point(_P2), add_point(rb ? _P4 : _P7), add_point(rb ? _P7 : _P0);
	// EndPrimitive();
}
