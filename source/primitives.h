#pragma once
#include "mesh.h"

enum class Primitive : u32 {
	QUAD = 0,
	CUBE = 1,
	MAX_MESH_ID = 2
};

Mesh_Any& get_primitive(Primitive type);

void clear_primitives();