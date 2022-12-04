#pragma once

#include <vector>
#include <functional>

#include "utils.h"
#include "mesh.h"

struct I_Triangle { u32 a, b, c; };
struct I_Line { u32 a, b; };
struct Line { vec3 a, b; };

struct Mesh_2D {
	std::vector<vec2> vertices;
	std::vector<I_Triangle> triangles;
	std::vector<I_Line> borderline;
};
struct Line_Set {
	std::vector<Line> lines{};
	float total_length = 0;
};
Line_Set get_border(const Mesh_2D* mesh_2d, float z);
std::vector<Particle> spawn_particles(const Line_Set* line_set, u32 amount, float spread);

Mesh_Any load_mesh(const Mesh_2D& mesh_2d);
struct Value_Map {
	std::vector<float> values;
	u32 X, Y;

	void fill(std::function<float(float, float)> f, u32 X = 10, u32 Y = 10);
	float at(u32 x, u32 y) const;
};

Mesh_2D make_mesh_2d(const Value_Map& value_map);

std::pair<Mesh_Any, Line_Set> make_layers_mesh(std::function<float(float, float, float)> f, u32 X = 10, u32 Y = 10, u32 Z = 10);

