#pragma once

#include "ResourceManager.hpp"
#include "ShaderClass.hpp"
#include <unordered_map>

class SceneManager{
public:
    uint32_t addEntity(const std::string& meshPath, glm::vec3 position, unsigned int shaderID){
        uint32_t id = nextEntityID++;

        // Get geometry data from resourcemgr
        meshes[id] = resourceMgr.getMesh(meshPath);
        meshes[id].shaderID = shaderID;

        // Transformation data
        transforms[id].pos = position;
        transforms[id].scale = glm::vec3(1.0f);
        transforms[id].rot = glm::vec3(0.0f);
        
        // Calculate the model matrix for the transform of the mesh
        recalculateMatrix_byID(id);        
        return id;
    }
    

    // Drawing will be done in RenderSystem. Below is the matrix recalculation function
    void recalculateMatrix_byID(unsigned int ID){
        transforms[ID].modelMatrix = glm::mat4(1.0f);
        transforms[ID].modelMatrix = glm::scale(transforms[ID].modelMatrix, transforms[ID].scale);
        transforms[ID].modelMatrix = glm::translate(transforms[ID].modelMatrix, transforms[ID].pos);
    }

    void recalculateMatrix(){
        for(auto& [id, t] : transforms){
            // only do this if the object doesn't have a parent
            updateNodeMatrix(id, glm::mat4(1.0f));
        }
    }

    void setParent(uint32_t childID, uint32_t parentID){
        // if either one of them doesn't exist, then just end things there
        if(!transforms.count(childID) || !transforms.count(parentID)) return;

        // Rest of the magic :relieved_emoji: 
        /*
            And the weather so breezy
            Man, why can't life always be this easy?
            She in the mirror dancin' so sleazy
            I get a call like, "Where are you, Yeezy?"
        */
        // Set child's parent
        transforms[childID].parent = parentID;
        // set parent's child / push to the vector
        transforms[parentID].children.push_back(childID);
        //------------------------------------THERE'S a POTENTIAL ISSUE HERE-----------------------------------//
        /*
            What if the childID is already in the array
        */

    }

    // Making unordered maps for meshes and other stuff for convenience
    // These have to be public in order to be accessible in the render system class
    std::unordered_map<uint32_t, MeshComponent> meshes;
    std::unordered_map<uint32_t, Transform> transforms;
    // 1 directional light max
    DirectionalLightComponent sun;
    // Only 1 Skybox
    SkyboxComponent sky;
    // multiple point lights and spot lights are possible so:
    std::unordered_map<uint32_t, PointLightComponent> pointLights;
    std::unordered_map<uint32_t, SpotLightComponents> spotLights;
private:
    uint32_t nextEntityID = 1;    
    ResourceManager resourceMgr;

    // See this shit? Recursive and shit man 
    void updateNodeMatrix(uint32_t entityID, const glm::mat4& parentWorldMatrix){
        Transform& t = transforms[entityID];

        glm::mat4 local = glm::mat4(1.0f);
        // Position
        local = glm::translate(local, t.pos);
        // Gonna skip rotation for now
        // Here's scaling
        local = glm::scale(local, t.scale);
        t.localMatrix = local;

        // Now we calculate the world matrix
        if(t.parent != 0){
            t.modelMatrix = parentWorldMatrix * t.localMatrix;
        }else{
            t.modelMatrix = t.localMatrix;
        }
        
        // Now we give all of this data belonging to the parent to all the children for free
        for(uint32_t childID : t.children){
            if(transforms[childID].parent != 0){
                updateNodeMatrix(childID, t.modelMatrix);
            }
        }
    }
};