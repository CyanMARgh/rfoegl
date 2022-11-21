#pragma once

#include "mesh.h"
#include "texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>

struct Model {
	std::vector<Mesh_N_UV> meshes;
	std::string directory;

	Model(const std::string& path);

	void load_model(const std::string& path);
	void process_node(aiNode *node, const aiScene *scene);
	Mesh_N_UV process_mesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> load_material_textures(aiMaterial *mat, aiTextureType type, std::string type_name);
};
void draw(const Model& model);