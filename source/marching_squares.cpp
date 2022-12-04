#include "marching_squares.h"
#include <cstdio>
#include <map>
#include <algorithm>

Line_Set get_border(const Mesh_2D* mesh_2d, float z) {
	Line_Set result;
	u32 n = mesh_2d->borderline.size();
	result.lines.resize(n);
	for(u32 i = 0; i < n; i++) {
		I_Line i_line = mesh_2d->borderline[i];
		vec2 a = mesh_2d->vertices[i_line.a], b = mesh_2d->vertices[i_line.b];
		Line line = {{a.x, 0.f, a.y}, {b.x, z, b.y}};
		result.lines[i] = line;
		result.total_length += glm::length(line.b - line.a);
	}
	return result;
}

void add_border(Line_Set* line_set, const Mesh_2D* mesh_2d, float z) {
	u32 n = mesh_2d->borderline.size();
	for(u32 i = 0; i < n; i++) {
		I_Line i_line = mesh_2d->borderline[i];
		vec2 a = mesh_2d->vertices[i_line.a], b = mesh_2d->vertices[i_line.b];
		Line line = {{a.x, z, a.y}, {b.x, z, b.y}};
		line_set->lines.push_back(line);
		line_set->total_length += glm::length(line.b - line.a);
	}
}

std::vector<Particle> spawn_particles(const Line_Set* line_set, u32 amount, float spread) {
	std::vector<float> positions(amount);
	for(u32 i = 0; i < amount; i++) { positions[i] = randf() * line_set->total_length; }
	std::sort(positions.begin(), positions.end());

	std::vector<Particle> result(amount);	
	u32 LC = line_set->lines.size();
	float l = 0.f;
	for(u32 i = 0, j = 0; i < amount; i++) {
		float dl = 0.f;
		float li = positions[i];
		while(1) {
			dl = glm::length(line_set->lines[j].b - line_set->lines[j].a);
			if(l + dl >= li || j == LC - 1) break;
			l += dl, ++j;
		}
		vec3 pos = j == LC - 1 ? line_set->lines[j].b : lerp(line_set->lines[j].a, line_set->lines[j].b, (li - l) / dl);
		result[i] = {pos + spread * (rand_vec3() * 2.f - 1.f)};
	}
	return result;
}
std::pair<Mesh_Any, Line_Set> make_layers_mesh(std::function<float(float, float, float)> f, u32 X, u32 Y, u32 Z) {
	if(Z < 2) { puts("should be at least 2 levels!\n"); exit(-1); }
	std::vector<Mesh_2D> meshes_2d(Z);
	std::map<std::pair<u32, u32>, u32> renames;
	auto add_id = [&renames] (u32 layer, u32 id) {
		auto F = renames.find({layer, id});
		if(F == renames.end()) renames[{layer, id}] = renames.size();
	};
	Line_Set line_set;
	u32 T = 0;
	for(u32 z = 0; z < Z; z++) {
		float zf = (float)z / (Z - 1);
		Value_Map value_map; value_map.fill([f, zf] (float x, float y) { return f(x, y, zf); }, X, Y);
		meshes_2d[z] = make_mesh_2d(value_map);
		T += meshes_2d[z].triangles.size();
		for(auto t : meshes_2d[z].triangles) {
			add_id(z, t.a), add_id(z, t.b), add_id(z, t.c);
		}
		add_border(&line_set, &(meshes_2d[z]), zf);
	}
	u32 V = renames.size();
	std::vector<u32> ids(T * 3);
	std::vector<Point_UV> points(V);
	for(u32 z = 0, t = 0; z < Z; z++) {
		u32 dt = meshes_2d[z].triangles.size();
		for(u32 i = 0; i < dt; i++) {
			I_Triangle tr = meshes_2d[z].triangles[i];
			ids[t + 3 * i] = renames[{z, tr.a}];
			ids[t + 3 * i + 1] = renames[{z, tr.b}];
			ids[t + 3 * i + 2] = renames[{z, tr.c}];
		}
		t += dt * 3;
	}
	for(auto [old_id, new_id] : renames) {
		vec2 v = meshes_2d[old_id.first].vertices[old_id.second];
		float zf = (float)(old_id.first) / (Z - 1);
		points[new_id] = {{v.x, zf, v.y}, {zf, 0.f}};
	}
	auto mesh_uv = make_mesh(link_mesh_uv, &(points[0]), V, &(ids[0]), T * 3);
	return {mesh_uv, std::move(line_set)};
}
Mesh_Any load_mesh(const Mesh_2D& mesh_2d) {
	std::unordered_map<u32, u32> renames;
	auto add_id  = [&renames] (u32 id) {
		auto F = renames.find(id);
		if(F == renames.end()) renames[id] = renames.size();
	};
	for(auto t : mesh_2d.triangles) {
		add_id(t.a), add_id(t.b), add_id(t.c);
	}

	u32 T = mesh_2d.triangles.size();
	u32 V = renames.size();

	std::vector<u32> ids(T * 3);
	std::vector<Point_UV> points(V);

	for(u32 i = 0; i < T; i++) {
		I_Triangle tr = mesh_2d.triangles[i];
		ids[3 * i] = renames[tr.a];
		ids[3 * i + 1] = renames[tr.b];
		ids[3 * i + 2] = renames[tr.c];
	}
	for(auto [old_id, new_id] : renames) {
		vec2 v = mesh_2d.vertices[old_id];
		points[new_id] = {{v.x, 0.f, v.y}};
	}

	return make_mesh(link_mesh_uv, &(points[0]), V, &(ids[0]), T * 3);
}
void Value_Map::fill(std::function<float(float, float)> f, u32 X, u32 Y) {
	this->X = X, this->Y = Y;
	if(X < 2 || Y < 2) {
		printf("too small value map\n");
		exit(-1);
	}
	values.resize(X * Y);
	for(u32 y = 0, i = 0; y < Y; y++) {
		for(u32 x = 0; x < X; x++, i++) {
			values[i] = f((float)x / (X - 1), (float)y / (Y - 1));
		}
	}
}	
float Value_Map::at(u32 x, u32 y) const {
	return values[x + y * X];
}
Mesh_2D make_mesh_2d(const Value_Map& value_map) {
	u32 X = value_map.X, Y = value_map.Y;
	u32 S_nodes = X * Y, S_hor = Y * (X - 1), S_ver = (Y - 1) * X, S_total = S_nodes + S_hor + S_ver;
	Mesh_2D result;
	result.vertices.resize(S_total);

	// nodes
	for(u32 y = 0, i = 0; y < Y; y++) {
		for(u32 x = 0; x < X; x++, i++) {
			float xf = (float)x / (X - 1), yf = (float)y / (Y - 1);
			result.vertices[i] = {xf, yf};
		}
	}
	// hor
	for(u32 y = 0, i = 0; y < Y; y++) {
		for(u32 x = 0; x < X - 1; x++, i++) {
			float xf = (float)x / (X - 1), yf = (float)y / (Y - 1);
			float h1 = value_map.at(x, y), h2 = value_map.at(x + 1, y);
			float t = h1 / (h1 - h2);
			result.vertices[S_nodes + i] = {xf + t / (X - 1), yf};
		}
	}
	// ver
	for(u32 y = 0, i = 0; y < Y - 1; y++) {
		for(u32 x = 0; x < X; x++, i++) {
			float xf = (float)x / (X - 1), yf = (float)y / (Y - 1);
			float h1 = value_map.at(x, y), h2 = value_map.at(x, y + 1);
			float t = h1 / (h1 - h2);
			result.vertices[S_nodes + S_hor + i] = {xf, yf + t / (Y - 1)};
		}
	}
	auto add_triangle = [&result] (u32 a, u32 b, u32 c) -> void {
		result.triangles.push_back({a, b, c});
	};
	auto add_line = [&result] (u32 a, u32 b) -> void {
		result.borderline.push_back({a, b});
	};
	for(u32 y = 0, i = 0; y < Y - 1; y++) {
		for(u32 x = 0; x < X - 1; x++, i++) {
			u8 type =
				(value_map.at(x, y) > 0) |
				((value_map.at(x + 1, y) > 0) << 1) |
				((value_map.at(x, y + 1) > 0) << 2) |
				((value_map.at(x + 1, y + 1) > 0) << 3);
			u32 tl = x + y * X, tr = x + 1 + y * X, bl = x + (y + 1) * X, br = x + 1 + (y + 1) * X;
			u32 t = S_nodes + x + y * (X - 1), b = S_nodes + x + (y + 1) * (X - 1);
			u32 l = S_nodes + S_hor + x + y * X, r = S_nodes + S_hor + x + 1 + y * X;
			switch(type) {
				case  0: break;
				case  1: add_triangle(tl, t, l), add_line(l, t); break;
				case  2: add_triangle(t, tr, r), add_line(t, r); break;
				case  3: add_triangle(tl, tr, r), add_triangle(tl, r, l), add_line(l, r); break;
				case  4: add_triangle(bl, l, b), add_line(b, l); break;
				case  5: add_triangle(tl, t, b), add_triangle(tl, b, bl), add_line(b, t); break;
				case  6: add_triangle(bl, l, b), add_triangle(t, tr, r), add_line(b, l), add_line(t, r); break;
				case  7: add_triangle(tl, tr, r), add_triangle(tl, r, b), add_triangle(tl, b, bl), add_line(b, r); break;
				case  8: add_triangle(r, br, b), add_line(r, b); break;
				case  9: add_triangle(r, br, b), add_triangle(l, tl, t), add_line(l, t), add_line(r, b); break;
				case 10: add_triangle(t, tr, br), add_triangle(t, br, b), add_line(t, b); break;
				case 11: add_triangle(tl, tr, br), add_triangle(tl, br, b), add_triangle(tl, b, l), add_line(l, b); break;
				case 12: add_triangle(l, r, br), add_triangle(l, br, bl), add_line(r, l); break;
				case 13: add_triangle(tl, t, r), add_triangle(tl, r, br), add_triangle(tl, br, bl), add_line(r, t); break;
				case 14: add_triangle(t, tr, br), add_triangle(t, br, bl), add_triangle(t, bl, l), add_line(t, l); break;
				case 15: add_triangle(tl, tr, br), add_triangle(tl, br, bl); break;
				default: printf("can't draw type #%d\n", type);
			}
		}
	}
	return result;
};
