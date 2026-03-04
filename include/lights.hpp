#pragma once

#include "ShaderClass.hpp"
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

class DirectionalLight {
  public:  
    void SetShader(Shader &SHADER);
    void setName(std::string NAME);

    std::string NAME;

    bool enabled = true;
    vec3 direction = vec3(-0.2f, -1.0f, -0.3f);
    vec3 color = vec3(1.0f);

    // Attribute Strength
    float specular = 1.0f;
    float diffuse = 1.0f;
    float ambient = 0.05f;

    // Shadow Mapping Data
    mat4 projection;
    mat4 view;

    void takeInput();

  private:
};

class PointLight {
  public:
    void SetShader(Shader &SHADER);
    void setName(std::string NAME);

    std::string NAME;

    bool enabled = true;
    vec3 position = vec3(0.0f);
    vec3 color = vec3(1.0f);

    // Attribute Strength
    float specular = 1.0f;
    float diffuse = 1.0f;
    float ambient = 0.02f;

    // Attenuation
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.0032f;

    // Shadow Mapping Data
    mat4 projection;
    mat4 view;

    void takeInput();

  private:
};

class SpotLight {

  public:

    SpotLight(glm::vec3 position);

    void SetShader(Shader &SHADER);
    void setName(std::string NAME);

    std::string NAME;

    bool enabled = true;
    vec3 position = vec3(0.0f);
    vec3 color = vec3(1.0f);

    // Attribute Strength
    float specular = 1.0f;
    float diffuse = 1.0f;
    float ambient = 0.02f;

    // SpotLight Variables
    float cutoff = sqrt(3) / 2;
    vec3 direction = vec3(0.0f, 0.0f, -1.0f);

    // Attenuation
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.0032f;
    void takeInput();

  private:
};
