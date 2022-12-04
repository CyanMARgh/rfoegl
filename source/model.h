#pragma once

#include "mesh.h"
#include "texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>

struct Model {
	// std::vector<Mesh_N_UV> meshes;
	std::vector<Mesh_Any> meshes;

	std::string directory;

	Model(const std::string& path);

	void load_model(const std::string& path);
	void process_node(aiNode *node, const aiScene *scene);
	Mesh_Any process_mesh(aiMesh *mesh, const aiScene *scene);
	void load_material_textures(std::vector<Texture>& textures, aiMaterial *mat, aiTextureType type, std::string type_name);
};
void draw(const Model& model, u32 uloc_diff, u32 uloc_spec, u32 uloc_norm);

void clear(Model& model);