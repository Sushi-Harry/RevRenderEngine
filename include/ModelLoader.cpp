#include "ModelLoader.hpp"
#include "ResourceManager.hpp"

MeshComponent ModelLoader::loadToGPU(const std::string& path, ResourceManager& resMgr){
    Assimp::Importer importer;
    // const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_OptimizeMeshes);
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_OptimizeMeshes);
    std::string directory = path.substr(0, path.find_last_of('/'));
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        std::cerr << "ASSIMP_ERROR::" << importer.GetErrorString() << std::endl;
        return {};
    }

    MeshComponent modelContainer;
    std::vector<VertexComponent> globalVertices;
    std::vector<uint32_t> globalIndices;
    ProcessNode(scene->mRootNode, scene, modelContainer, directory, resMgr, globalVertices, globalIndices);

    //---------------ADD MESH DATA TO THE PHYSICS ENGINE----------------//
    resMgr.addPhysicsMeshData(path, globalVertices, globalIndices);

    return modelContainer;
}

SubMesh ModelLoader::uploadMesh(aiMesh* mesh, const aiScene* scene, const std::string& directory, ResourceManager& resMgr, std::vector<VertexComponent>& globalVertices, std::vector<uint32_t>& globalIndices){
    std::vector<VertexComponent> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureComponent> textures;

    // Vertex data
    for(unsigned int i = 0; i < mesh->mNumVertices; i++){
            VertexComponent vertex;
            glm::vec3 pos, norm;
            // Position attribute
            pos.x = mesh->mVertices[i].x;
            pos.y = mesh->mVertices[i].y;
            pos.z = mesh->mVertices[i].z;
            // Normals
            norm.x = mesh->mNormals[i].x;
            norm.y = mesh->mNormals[i].y;
            norm.z = mesh->mNormals[i].z;
            // Setting the vertex attributes
            vertex.position = pos;
            vertex.normal = norm;
            if(mesh->mTextureCoords[0]){
                glm::vec2 tex;
                tex.x = mesh->mTextureCoords[0][i].x;
                tex.y = mesh->mTextureCoords[0][i].y;

                vertex.texCoords = tex;
            }else{
                vertex.texCoords = glm::vec2(0.0f, 0.0f);
            }
            vertices.push_back(vertex);
        }
        //Face processing for indices
        for(unsigned int i = 0; i < mesh->mNumFaces; i++){
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++){
                indices.push_back(face.mIndices[j]);
            }
        }

        // Extraction of vertex data and applying proper offsets to give the data to the physics engine eventually
        unsigned int offset = globalVertices.size();
        // inserting the vertex data into the global vertices
        globalVertices.insert(globalVertices.end(), vertices.begin(), vertices.end());
        // inserting the index data into the global indices
        for(unsigned int idx : indices){
            globalIndices.push_back(idx + offset);
        }

        // Texture data
        unsigned int textureID = 0;
        if(mesh->mMaterialIndex >= 0){
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0){
                aiString str;
                material->GetTexture(aiTextureType_DIFFUSE, 0, &str);

                std::string fullPath = directory + "/" + std::string(str.C_Str());

                textureID = resMgr.getTexture(fullPath);
            }
        }
        if(textureID == 0){
            textureID = ResourceManager::getDefaultTexture();
        }

        // Standard VAO, VBO, EBO shit
        unsigned int VAO, VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        
        glBindVertexArray(VAO);
        
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexComponent), &vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

            // Vertices
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexComponent), (void*)0);
            //Normals
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexComponent), (void*)offsetof(VertexComponent, normal));
            //Texture Coords
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexComponent), (void*)offsetof(VertexComponent, texCoords));
        
        glBindVertexArray(0);

    // Returning the mesh Component
    return { VAO, (unsigned int)indices.size(), textureID};
}

void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, MeshComponent& container, const std::string& directory, ResourceManager& resMgr, std::vector<VertexComponent>& globalVertices, std::vector<uint32_t>& globalIndices){
    // Processing all the meshes first
    for(unsigned int i = 0; i < node->mNumMeshes; i++){
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        container.subMeshes.push_back(uploadMesh(mesh, scene, directory, resMgr, globalVertices, globalIndices));
    }

    // Recursively process children
    for(unsigned int i = 0; i < node->mNumChildren; i ++){
        ProcessNode(node->mChildren[i], scene, container, directory, resMgr, globalVertices, globalIndices);
    }
}