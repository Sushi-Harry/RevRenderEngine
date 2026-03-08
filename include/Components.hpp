#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

#include <reactphysics3d/reactphysics3d.h>

// Components
struct SubMesh{
    unsigned int VAO;
    unsigned int indexCount;
    unsigned int textureID;
};

struct MeshComponent
{
    std::vector<SubMesh> subMeshes;
    unsigned int shaderID;
    bool isSelected = false;
};

struct TextureComponent{
    unsigned int ID;
    std::string path;
    std::string type;
};

struct VertexComponent{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Transform{
    glm::vec3 pos = glm::vec3(0.0f);
    glm::vec3 rot = glm::vec3(0.0f);;
    glm::vec3 scale = glm::vec3(1.0f);

    // Quaternion for later use
    glm::quat rotationQuaternion;
    // Bound to the world or the parent 
    glm::mat4 localMatrix = glm::mat4(1.0f);
    // Final calculated matrix
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // 0 = root object / no parent
    uint32_t parent = 0;
    std::vector<uint32_t> children;

};

// Lighting COmponents
struct DirectionalLightComponent{
    bool enabled = true;
    glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    glm::vec3 color = glm::vec3(1.0f);
    // lighting variables
    float specular = 1.0f;
    float diffuse = 1.0f;
    float ambient = 0.05f;
};

struct PointLightComponent{
    bool enabled = false;
    glm::vec3 color = glm::vec3(1.0f);
    glm::vec3 position = glm::vec3(0.0f);
    //lighting variables
    float specular = 1.0f;
    float diffuse = 1.0f;
    float ambient = 0.02f;
    //attenuation variables
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.0032f;  
};

struct SpotLightComponents{
    bool enabled = false;
    
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

    float diffuse = 1.0f;
    float specular = 1.0f;
    float ambient = 0.0f;

    float cutoff = glm::cos(glm::radians(12.0f));

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.0032f;
};

struct SkyboxComponent{
    bool enabled = false;
    unsigned int VAO;
    unsigned int cubemapID = 0;
    unsigned int shaderID = 0;
};

struct RigidBodyComponent{
    rp3d::RigidBody* body = nullptr;
    rp3d::CollisionShape* shape = nullptr;
};