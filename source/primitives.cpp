#include "primitives.h"

Point_UV quad_points[] = {
	{{-1, -1,  0},  {0, 0}},
	{{ 1, -1,  0},  {1, 0}},
	{{ 1,  1,  0},  {1, 1}},
	{{-1,  1,  0},  {0, 1}},
};
u32 quad_ids[] = {0, 1, 2, 0, 2, 3};
Point_UV cube_points[] = {
	{{-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f}},
	{{-0.5f,  0.5f, -0.5f},  {1.0f, 0.0f}},
	{{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}},
	{{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},

	{{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}},
	{{ 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}},
	{{ 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f}},
	{{-0.5f,  0.5f,  0.5f},  {0.0f, 1.0f}},

	{{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}},
	{{-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
	{{-0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}},
	{{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},

	{{ 0.5f,  0.5f, -0.5f},  {0.0f, 0.0f}},
	{{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
	{{ 0.5f, -0.5f,  0.5f},  {1.0f, 1.0f}},
	{{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},

	{{ 0.5f, -0.5f, -0.5f},  {0.0f, 0.0f}},
	{{ 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}},
	{{-0.5f, -0.5f,  0.5f},  {1.0f, 1.0f}},
	{{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},

	{{-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f}},
	{{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
	{{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}},
	{{-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f}}
};
u32 cube_ids[] = {
	 0,  1,  2,  0,  2,  3,
	 4,  5,  6,  4,  6,  7,
	 8,  9, 10,  8, 10, 11,
	 12, 13, 14, 12, 14, 15,
	 16, 17, 18, 16, 18, 19,
	 20, 21, 22, 20, 22, 23
};

const u32 MAX_MESHES = (u32)Primitive::MAX_MESH_ID;

Mesh_Raw meshes_raw[MAX_MESHES] = {
	{(float*)quad_points, quad_ids, 4, 6},
	{(float*)cube_points, cube_ids, 24, 36},	
};
Mesh_Any primitives[MAX_MESHES];
bool is_loaded[MAX_MESHES] = {false, false};

Mesh_Any& get_primitive(Primitive type) {
	u32 id = (u32)type;
	if(!is_loaded[id]) {
		primitives[id] = make_mesh<Point_UV>(meshes_raw[id]);
		is_loaded[id] = true;
	}
	return primitives[id];
	// return make_mesh<Point_UV>(meshes_raw[id]);
}
void clear_primitives() {
	for(u32 i = 0; i < MAX_MESHES; i++) {
		is_loaded[i] = false;
		primitives[i] = {};
	}	
}