#pragma once

#include <vector>
#include <string>

#include "Mesh.h"

#include <assimp/scene.h>


using std::vector;
using std::string;


class Model
{
	public:
		vector<Mesh> meshes;
		string directory;

		Model(string const& path);

		void Draw();

	private:
		void loadModel(string const& path);

		void processNode(aiNode* node, const aiScene* scene);

		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};

