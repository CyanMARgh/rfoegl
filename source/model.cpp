#include "model.h"

#include "mesh.h"
#include "texture.h"
#include "utils.h"

#include <vector>

// struct Model {
// 	std::vector<Mesh_N_UV> meshes;
// 	std::string directory;

Model::Model(const std::string& path) {
	load_model(path);
}

void Model::load_model(const std::string& path) {
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		printf("error while loading model(%s): %s\n", path.c_str(), importer.GetErrorString());
		exit(-1);
	}
	directory = path.substr(0, path.find_last_of('/'));
	process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode *node, const aiScene *scene) {
	for(u32 i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(process_mesh(mesh, scene));
	}
	for(u32 i = 0; i < node->mNumChildren; i++) {
		process_node(node->mChildren[i], scene);
	}
}

Mesh_N_UV Model::process_mesh(aiMesh *mesh, const aiScene *scene) {
	std::vector<Point_N_UV> vertices(mesh->mNumVertices);
	std::vector<u32> ids;

	for(u32 i = 0; i < mesh->mNumVertices; i++) {
		vertices[i].pos = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z }; 
	}
	if(mesh->mTextureCoords[0]) {
		for(u32 i = 0; i < mesh->mNumVertices; i++) {
			vertices[i].uv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
		}
	}
	for(u32 i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		if(face.mNumIndices != 3) {
			printf("face with %d indices!\n", face.mNumIndices); 
			exit(-1);
		}
		for(u32 j = 0; j < face.mNumIndices; j++) {
			ids.push_back(face.mIndices[j]);
		}
	}
	if(mesh->mNormals) {
		for(u32 i = 0; i < mesh->mNumVertices; i++) {
			vertices[i].norm = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
		}
	} else {
		for(u32 i = 0; i < ids.size(); i+=3) {
			Point_N_UV
				&a = vertices[ids[i]],
				&b = vertices[ids[i + 1]],
				&c = vertices[ids[i + 2]];
			vec3 n = normalize(glm::cross(b.pos - a.pos, c.pos - b.pos));
			a.norm += n;
			b.norm += n;
			c.norm += n;
		}
		for(u32 i = 0; i < mesh->mNumVertices; i++) {
			vertices[i].norm = normalize(vertices[i].norm);
		}
	}
	std::vector<Texture> textures;
	if(mesh->mMaterialIndex >= 0) {
		aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
		load_material_textures(textures, mat, aiTextureType_DIFFUSE, "texture_diffuse");
		load_material_textures(textures, mat, aiTextureType_SPECULAR, "texture_specular");
	}
	// printf("size = %d\n", vertices.size());
	return Mesh_N_UV(&(vertices[0]), vertices.size(), &(ids[0]), ids.size(), textures);
}

void Model::load_material_textures(std::vector<Texture>& textures, aiMaterial *mat, aiTextureType type, std::string type_name) {
	for(u32 i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str0;
		mat->GetTexture(type, i, &str0);
		Texture texture;
		std::string str1 = str0.C_Str();
		texture = load_texture(directory + "/" + str1);
		textures.push_back(texture);
	}
}

void draw(const Model& model, u32 uloc_diff, u32 uloc_spec) {
	for(const auto& mesh : model.meshes) { draw(mesh, uloc_diff, uloc_spec); }
}