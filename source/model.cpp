#include "model.h"

#include "mesh.h"
#include "texture.h"
#include "utils.h"

#include <vector>
#include <set>

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
		// printf("here!\n");
		for(u32 i = 0; i < mesh->mNumVertices; i++) {
			vertices[i].n1 = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
		}
	} else {
		std::set<u32> s_ids = {};

		for(u32 i = 0; i < ids.size(); i+=3) {
			s_ids.insert(ids[i]);
			s_ids.insert(ids[i+1]);
			s_ids.insert(ids[i+2]);
			Point_N_UV
				&a = vertices[ids[i]],
				&b = vertices[ids[i + 1]],
				&c = vertices[ids[i + 2]];
			vec3 n1 = normalize(glm::cross(b.pos - a.pos, c.pos - b.pos));
			a.n1 += n1; b.n1 += n1; c.n1 += n1;
		}
		// printf("[%ld / %ld]\n", s_ids.size(), ids.size());
	}
	if(mesh->mTextureCoords[0]) {
		for(u32 i = 0; i < ids.size(); i += 3) {
			Point_N_UV
				&a = vertices[ids[i]],
				&b = vertices[ids[i + 1]],
				&c = vertices[ids[i + 2]];
			vec3 e1 = b.pos - a.pos, e2 = c.pos - a.pos;
			vec2 duv1 = b.uv - a.uv, duv2 = c.uv - a.uv;
			
			float f = 1.f / (duv1.x * duv2.y - duv2.x * duv1.y);

			vec3 n2, n3;

			n2.x = (duv2.y * e1.x - duv1.y * e2.x);
			n2.y = (duv2.y * e1.y - duv1.y * e2.y);
			n2.z = (duv2.y * e1.z - duv1.y * e2.z);
			n2 = normalize(n2 * f);

			n3.x = (-duv2.x * e1.x + duv1.x * e2.x);
			n3.y = (-duv2.x * e1.y + duv1.x * e2.y);
			n3.z = (-duv2.x * e1.z + duv1.x * e2.z);
			n3 = normalize(n3 * f);

			a.n2 = n2; b.n2 = n2; c.n2 = n2;
			a.n3 = n3; b.n3 = n3; c.n3 = n3;
		}
	}
	for(u32 i = 0; i < mesh->mNumVertices; i++) {
		vertices[i].n1 = normalize(vertices[i].n1);
		// vertices[i].n2 = normalize(vertices[i].n2);
		// vertices[i].n3 = normalize(vertices[i].n3);
	}

	std::vector<Texture> textures;
	if(mesh->mMaterialIndex >= 0) {
		aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
		load_material_textures(textures, mat, aiTextureType_DIFFUSE, "texture_diffuse");
		load_material_textures(textures, mat, aiTextureType_SPECULAR, "texture_specular");
		load_material_textures(textures, mat, aiTextureType_HEIGHT, "texture_normals");
	}
	// printf("size = %d\n", vertices.size());
	return Mesh_N_UV(&(vertices[0]), vertices.size(), &(ids[0]), ids.size(), textures);
}

void Model::load_material_textures(std::vector<Texture>& textures, aiMaterial *mat, aiTextureType type, std::string type_name) {
	// printf("loading textures of %s, total = %d\n", directory.c_str(), mat->GetTextureCount(type));
	for(u32 i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str0;
		mat->GetTexture(type, i, &str0);
		Texture texture;
		std::string str1 = str0.C_Str();
		texture = load_texture(directory + "/" + str1, type);
		textures.push_back(texture);
	}
}

void draw(const Model& model, u32 uloc_diff, u32 uloc_spec, u32 uloc_norm) {
	for(const auto& mesh : model.meshes) { draw(mesh, uloc_diff, uloc_spec, uloc_norm); }
}