#pragma once

#include <iostream>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ModelLoader.hpp"
#include "TextureLoader.hpp"
#include "ShaderClass.hpp"

class ResourceManager{
public:
    MeshComponent getMesh(const std::string& path){
        if(cache.find(path) == cache.end()){
            cache[path] = loader.loadToGPU(path, *this);
        }
        return cache[path];
    }
    unsigned int getTexture(const std::string& path){
        if(texture_cache.find(path) != texture_cache.end()){
            return texture_cache[path];
        }

        unsigned int newID = TextureLoader::LoadTexture(path);
        texture_cache[path] = newID;
        return newID;
    }

    // This is a very clever trick. Instead of using a 1K resolution white image for the default texture, I'll just generate 1 PIXEL that's completely white
    static unsigned int getDefaultTexture(){
        static unsigned int defaultTexture = 0;

        if(defaultTexture == 0){
            glGenTextures(1, &defaultTexture);
            glBindTexture(GL_TEXTURE_2D, defaultTexture);

            unsigned char whitePixel[] = {255, 255, 255, 255 };

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
            // it's a 1x1 image so filtering probably doesn't matter but I'll go ahead and do it anyway
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        return defaultTexture;
    }

    unsigned int loadShader(const std::string& name, const char* vs_path, const char* fs_path){
        if(shaderCache.find(name) == shaderCache.end()){
            shaderCache[name] = new Shader(vs_path, fs_path);
            shaderIDs[name] = shaderCache[name]->ID;
            shaderByID[shaderIDs[name]] = shaderCache[name];
        }
        return shaderIDs[name];
    }
    
    Shader* getShader(unsigned int id){
        if(shaderByID.find(id) != shaderByID.end()){
            return shaderByID[id];
        }
        return nullptr;
    }

    unsigned int loadCubemap(const std::string& name, std::vector<std::string> faces){
        if(cubemap_cache.find(name) != cubemap_cache.end()){
            return cubemap_cache[name];
        }

        unsigned int newID = TextureLoader::LoadCubeMap(faces);
        cubemap_cache[name] = newID;
        return newID;
    }
private:
    ModelLoader loader;
    std::unordered_map<std::string, MeshComponent> cache;
    std::unordered_map<std::string, unsigned int> texture_cache;
    std::unordered_map<std::string, Shader*> shaderCache;
    std::unordered_map<std::string, unsigned int> shaderIDs;
    std::unordered_map<unsigned int, Shader*> shaderByID;
    std::unordered_map<std::string, unsigned int> cubemap_cache;
    unsigned int nextShader = 1;
};