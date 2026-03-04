#include "lights.hpp"
#include "ShaderClass.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"

// --------------------------------------DIRECTIONAL LIGHT------------------------------------//

void DirectionalLight::SetShader(Shader &SHADER){
    SHADER.setBool("dirLight.enabled", enabled);
    SHADER.setVec3("dirLight.color", color);
    SHADER.setVec3("dirLight.direction", direction);
    SHADER.setFloat("dirLight.specular", specular);
    SHADER.setFloat("dirLight.ambient", ambient);
    SHADER.setFloat("dirLight.diffuse", diffuse);
}

void DirectionalLight::takeInput(){

    if(NAME.empty()){
        std::cout << "NAME NOT ASSIGNED" << std::endl;
        return;
    }

    ImGui::Text("DIRECTIONAL LIGHT CONTROLS");
    ImGui::Checkbox("Enable Directional Light", &enabled);
    ImGui::DragFloat3("Rotation", glm::value_ptr(direction));
    ImGui::DragFloat("Directional Light Ambient Coefficient", &ambient);
    ImGui::ColorEdit3("Color", glm::value_ptr(color));
}

void DirectionalLight::setName(std::string NAME){
    this->NAME = NAME;
}

//----------------------------------------POINT-LIGHT----------------------------------------//

void PointLight::SetShader(Shader &SHADER){
    SHADER.setBool("pntLight.enabled", enabled);
    SHADER.setVec3("pntLight.color", color);
    SHADER.setFloat("pntLight.specular", specular);
    SHADER.setFloat("pntLight.diffuse", diffuse);
    SHADER.setFloat("pntLight.ambient", ambient);
    SHADER.setVec3("pntLight.position", position);

    // ATTENUATION VARIABLES
    SHADER.setFloat("pntLight.constant", constant);
    SHADER.setFloat("pntLight.linear", linear);
    SHADER.setFloat("pntLight.quadratic", quadratic);
}

void PointLight::takeInput(){

    if(NAME.empty()){
        std::cout << "NAME NOT ASSIGNED" << std::endl;
        return;
    }

    ImGui::Text("POINT LIGHT CONTROLS");
    ImGui::Checkbox("Enable Point Light", &enabled);
    ImGui::DragFloat3("Point Light Position", glm::value_ptr(position));
    ImGui::DragFloat("Point Light Ambient Coefficient", &ambient);
    ImGui::ColorEdit3("Point Light Color", glm::value_ptr(color));
}

void PointLight::setName(std::string NAME){
    this->NAME = NAME;
}

//----------------------------------------SPOT-LIGHT--------------------------------------------//
SpotLight::SpotLight(glm::vec3 position){
    this->position = position;
}

void SpotLight::SetShader(Shader &SHADER){
    SHADER.setBool("SpotLight1.enabled", enabled);
    SHADER.setVec3("SpotLight1.position", position);
    SHADER.setVec3("SpotLight1.direction", direction);
    SHADER.setVec3("SpotLight1.color", color);
    SHADER.setFloat("SpotLight1.specular", specular);
    SHADER.setFloat("SpotLight1.ambient", ambient);
    SHADER.setFloat("SpotLight1.diffuse", diffuse);
    SHADER.setFloat("SpotLight1.cutoff", cutoff);

    // ATTENUATION VARIABLES
    SHADER.setFloat("SpotLight1.constant", constant);
    SHADER.setFloat("SpotLight1.linear", linear);
    SHADER.setFloat("SpotLight1.quadratic", quadratic);
}

void SpotLight::takeInput(){

    if(NAME.empty()){
        std::cout << "NAME NOT ASSIGNED" << std::endl;
        return;
    }

    ImGui::Text("SPOT-LIGHT CONTROLS");
    ImGui::Checkbox("Enable Spot Light", &enabled);
    ImGui::DragFloat3("Spot-Light Position", glm::value_ptr(position));
    ImGui::ColorEdit3("Spot-Light Color", glm::value_ptr(color));
    ImGui::DragFloat3("Spot-Light Direction", glm::value_ptr(direction));
    ImGui::DragFloat("Spot-Light Ambient Coefficient", &ambient);
    ImGui::SliderFloat("Cut-Off Angle Cosine", &cutoff, 0.0f, 0.9f);
}

void SpotLight::setName(std::string NAME){
    this->NAME = NAME;
}