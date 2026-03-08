#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext/vector_uint4.hpp>
#include "ShaderClass.hpp"
#include "camera.h"
#include "ResourceManager.hpp"
#include "SceneManager.hpp"
#include "RenderSystem.hpp"
#include "PixelPicker.hpp"
#include <reactphysics3d/reactphysics3d.h>

class Engine{
public:
    Engine(int HEIGHT, int WIDTH, const char* WINDOW_NAME);
    static void framebuffer_size_callback(GLFWwindow* WINDOW, int WIDTH, int HEIGHT);
    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    void Update();
    bool isRunning();
private:
    GLFWwindow* window;
    Camera* MainCamera;
    glm::mat4 projection;
    glm::mat4 view;
    // Input Variables
    bool CURSOR_HIDDEN;
    bool TAB_WAS_PRESSED;

    int width, height; // WINDOW VARIABLES
    // Delta Time Implementation
    float lastX, lastY;
    bool firstMouse;
    float deltaTime = 0.0f;
    float lastFrame;

    bool leftMousePressed = false;
    bool isDragging = false;
    unsigned int selectedModelID = 0;
    float dragDepth = 0.0f;
    glm::vec3 grabOffset = glm::vec3(0.0f);

    // React Physics 3D
    rp3d::PhysicsCommon physicsCommon;
    rp3d::PhysicsWorld* physicsWorld;

    // Rewrite Testing
    ResourceManager resourceMgr;
    SceneManager sceneMgr;
    RenderSystem renderer;
    PixelPicker* picker;
    uint32_t testEntityID; // Keep track of the test object
    uint32_t testEntity2;

    // Skybox Rendering
    unsigned int generateSkyboxVAO();

    // Shadow Mapping
    Shader* shadowShader;
    ShadowMapFBO* directionalShadowMap;

    // Object Selection
    unsigned int pixelPickingShaderID;
    Shader* OutlineShader;
    void BufferSetup();

    // Private Utility Functions
    bool initGLFW();
    bool initGLEW();
    void CameraSetup();
    void DeltaTimeCalculation();
    void ProcessMovement(GLFWwindow* window);
    glm::vec3 getWorldPosFromMouse(double mouseX, double mouseY, float dragDepth);
    
    // Selection functions
    void PerformSelection();
    void DeselectAll();

    // Physics Simulation
    float accumulator = 0.0f;
    void SyncPhysicsToGraphics();
    unsigned int debugVAO, debugVBO, debugShaderID;
    void debug_init_physics();
    void debug_render_physics();

};