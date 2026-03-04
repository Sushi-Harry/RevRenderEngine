#pragma once

#include <GL/glew.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Components.hpp"

class ResourceManager;

class ModelLoader{
public:
    MeshComponent loadToGPU(const std::string& path, ResourceManager& resMgr);
private:
    SubMesh uploadMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory, ResourceManager& resMgr);
    
    void ProcessNode(aiNode* node, const aiScene* scene, MeshComponent& container, const std::string& directory, ResourceManager& resMgr);
};