#pragma once
#include "SceneManager.hpp"
#include "camera.h"
#include "ShadowMap.hpp"

class RenderSystem{
public:
    void draw(SceneManager& scene, ResourceManager& resMgr, Camera& cam, Shader& outlineShader, Shader& shadowShader, ShadowMapFBO& shadowFBO, int width, int height){
        
        //--------------------------------------------------------------//
        //   0th RENDER PASS                                            //
        //--------------------------------------------------------------//
        glm::mat4 lightSpaceMatrix = getDirectionalLightSpaceMatrix(scene.sun);
        shadowShader.use();
        shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        shadowFBO.bind();
        for(auto& [id, mesh] : scene.meshes){
            if(scene.transforms.count(id)){
                shadowShader.setMat4("model", scene.transforms[id].modelMatrix);

                for(auto& subm : mesh.subMeshes){
                    glBindVertexArray(subm.VAO);
                    glDrawElements(GL_TRIANGLES, subm.indexCount, GL_UNSIGNED_INT, 0);
                }
            }
        }
        shadowFBO.unbind(width, height);

        // Prepping for the stencil testing
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        //--------------------------------------------------------------//
        // FIRST RENDER PASS                                            //
        //--------------------------------------------------------------//
        unsigned int boundShaderID = 0;
        Shader* activeShader = nullptr;

        for(auto& [id, mesh] : scene.meshes){
            if(scene.transforms.count(id)){

                if(mesh.isSelected){
                    glStencilFunc(GL_ALWAYS, 1, 0xFF);
                    glStencilMask(0xFF);
                } else {
                    glStencilFunc(GL_ALWAYS, 0, 0xFF); // Reset to 0 for unselected
                    glStencilMask(0x00); // Lock the buffer
                }

                // Selecting the active shader
                if(mesh.shaderID != boundShaderID){
                    activeShader = resMgr.getShader(mesh.shaderID);
                    if(activeShader){
                        activeShader->use();
                        activeShader->setMat4("view", cam.GetViewMatrix());
                        activeShader->setMat4("projection", cam.GetProjectionMatrix());
                        activeShader->setVec3("viewPos", cam.Position);
                        activeShader->setFloat("material.shininess", 32.0f);

                        //======================================================//
                        //===========Directional Light Uniforms=================//
                        //======================================================//
                        activeShader->setBool("dirLight.enabled", scene.sun.enabled);
                        activeShader->setVec3("dirLight.direction", scene.sun.direction);
                        activeShader->setVec3("dirLight.color", scene.sun.color);
                        activeShader->setFloat("dirLight.ambient", scene.sun.ambient);
                        activeShader->setFloat("dirLight.diffuse", scene.sun.diffuse);
                        activeShader->setFloat("dirLight.specular", scene.sun.specular);
                        //--------------Light Space Matrix----------------------//
                        activeShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

                        //======================================================//
                        //==============Point Light Uniforms====================//
                        //======================================================//
                        bool foundPointLights = false;
                        for(auto& [id, pLight] : scene.pointLights){
                            if(pLight.enabled && scene.transforms.count(id)){
                                activeShader->setBool("pntLight.enabled", true);
                                activeShader->setVec3("pntLight.position", scene.transforms[id].pos); // Taken right from the Transform!
                                activeShader->setVec3("pntLight.color", pLight.color);
                                activeShader->setFloat("pntLight.ambient", pLight.ambient);
                                activeShader->setFloat("pntLight.diffuse", pLight.diffuse);
                                activeShader->setFloat("pntLight.specular", pLight.specular);
                                activeShader->setFloat("pntLight.constant", pLight.constant);
                                activeShader->setFloat("pntLight.linear", pLight.linear);
                                activeShader->setFloat("pntLight.quadratic", pLight.quadratic);

                                foundPointLights = true;
                                break;
                            }
                        }
                        if(!foundPointLights){
                            activeShader->setBool("pntLight.enabled", false);
                        }
                        bool foundSpotLights = false;
                        for(auto& [id, sLight] : scene.spotLights){
                            if(sLight.enabled && scene.transforms.count(id)){
                                activeShader->setBool("SpotLight1.enabled", true);
                                activeShader->setVec3("SpotLight1.position", scene.transforms[id].pos); 
                                activeShader->setVec3("SpotLight1.direction", sLight.direction);
                                activeShader->setVec3("SpotLight1.color", sLight.color);
                                activeShader->setFloat("SpotLight1.ambient", sLight.ambient);
                                activeShader->setFloat("SpotLight1.diffuse", sLight.diffuse);
                                activeShader->setFloat("SpotLight1.specular", sLight.specular);
                                activeShader->setFloat("SpotLight1.cutoff", sLight.cutoff);
                                activeShader->setFloat("SpotLight1.constant", sLight.constant);
                                activeShader->setFloat("SpotLight1.linear", sLight.linear);
                                activeShader->setFloat("SpotLight1.quadratic", sLight.quadratic);

                                foundSpotLights = true;
                                break;
                            }
                        }
                        if(!foundSpotLights){
                            activeShader->setBool("SpotLight1.enabled", false);
                        }                        
                        
                        boundShaderID = mesh.shaderID;
                    }
                }

                // Drawing using the active shader
                if(activeShader){
                    Transform& t = scene.transforms[id];
                    activeShader->setMat4("model", t.modelMatrix);

                    for(auto& subm : mesh.subMeshes){
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, subm.textureID);
                        activeShader->setInt("material.texture_diffuse1", 0);
                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, resMgr.getDefaultTexture());
                        activeShader->setInt("material.texture_specular1", 1);
                        glActiveTexture(GL_TEXTURE2);
                        glBindTexture(GL_TEXTURE_2D, shadowFBO.depthMap);
                        activeShader->setInt("shadowMap", 2);

                        glBindVertexArray(subm.VAO);
                        glDrawElements(GL_TRIANGLES, subm.indexCount, GL_UNSIGNED_INT, 0);
                    }
                }
            }
        }

        drawSkybox(scene, resMgr, cam);

        //--------------------------------------------------------------//
        //SECOND RENDER PASS                                            //
        //--------------------------------------------------------------//
        // draw only where stencil buffer != 1
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00); // Disable drawing to the buffer
        glDisable(GL_DEPTH_TEST);
        
        outlineShader.use();
        outlineShader.setMat4("view", cam.GetViewMatrix());
        outlineShader.setMat4("projection", cam.GetProjectionMatrix());
        for(auto& [id, mesh] : scene.meshes){
            if(mesh.isSelected && scene.transforms.count(id)){
                Transform& t = scene.transforms[id];

                // Scale the model matrix by a little bit
                glm::mat4 outlineMatrix = glm::scale(t.modelMatrix, glm::vec3(1.05f));
                outlineShader.setMat4("model", outlineMatrix);

                for(auto& subm : mesh.subMeshes){
                    glBindVertexArray(subm.VAO);
                    glDrawElements(GL_TRIANGLES, subm.indexCount, GL_UNSIGNED_INT, 0);
                }
            }
        }
        //--------------------------------------------------------------//
        //RESET THE OPENGL STATE TO DEFAULT VALUES                      //
        //--------------------------------------------------------------//
        glBindVertexArray(0);
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);
    }

    void pixelPickerRenderPass(SceneManager& scene, Shader& pickingShader, Camera& cam){
        pickingShader.use();
        pickingShader.setMat4("view", cam.GetViewMatrix());
        pickingShader.setMat4("projection", cam.GetProjectionMatrix());

        for(auto& [id, mesh] : scene.meshes){
            if(scene.transforms.count(id)){
                Transform& t = scene.transforms[id];
                pickingShader.setMat4("model", t.modelMatrix);
                float r = (id & 0x000000FF) >> 0;
                float g = (id & 0x0000FF00) >> 8;
                float b = (id & 0x00FF0000) >> 16;
                pickingShader.setVec3("PickingColor", glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f));

                for(auto& subm : mesh.subMeshes){
                    glBindVertexArray(subm.VAO);
                    glDrawElements(GL_TRIANGLES, subm.indexCount, GL_UNSIGNED_INT, 0);
                }
            }
        }
        glBindVertexArray(0);
    }

    void drawSkybox(SceneManager& scene, ResourceManager& resMgr, Camera& cam){
        if(!scene.sky.enabled || scene.sky.VAO == 0 || scene.sky.cubemapID == 0) return;

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        glStencilMask(0x00);

        Shader* skyShader = resMgr.getShader(scene.sky.shaderID);
        if(skyShader){
            skyShader->use();
            glm::mat4 view = glm::mat4(glm::mat3(cam.GetViewMatrix()));
            skyShader->setMat4("view", view);
            skyShader->setMat4("projection", cam.GetProjectionMatrix());

            glBindVertexArray(scene.sky.VAO);
            glBindTexture(GL_TEXTURE_CUBE_MAP, scene.sky.cubemapID);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
        }

    }

    // LightSpaceMatrix for sun / directional lights
    glm::mat4 getDirectionalLightSpaceMatrix(const DirectionalLightComponent& sun){
        // 1. Ortho Projection
        float near_plane = 1.0f, far_plane = 50.0f;
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

        glm::vec3 lightPos = -sun.direction * 20.0f;
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        return lightProjection * lightView;
    }
};