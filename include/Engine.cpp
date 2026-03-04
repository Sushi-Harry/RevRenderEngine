#include "Engine.hpp"
#include "ShaderClass.hpp"
#include "camera.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Engine::Engine(int HEIGHT, int WIDTH, const char *WINDOW_NAME) {

    // For later use
    height = HEIGHT;
    width = WIDTH;

    CURSOR_HIDDEN = true;
    TAB_WAS_PRESSED = false;

    // initialize glfw
    if (!initGLFW()) {
        std::cout << "Couldn't initialize GLFW\n" << std::endl;
        return;
    }

    // Create a winndow
    window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_NAME, NULL, NULL);
    if (!window) {
        std::cout << "Couldn't initialize the window" << std::endl;
        return;
    }
    glfwMakeContextCurrent(window);

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, Engine::framebuffer_size_callback);
    glfwSetCursorPosCallback(window, Engine::mouse_callback);
    glfwSetMouseButtonCallback(window, Engine::mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    // initialize GLEW
    if (!initGLEW()) {
        std::cout << "Couldn't Initialize GLEW\n" << std::endl;
        return;
    }
    glViewport(0, 0, WIDTH, HEIGHT);
    // Remove framelimit
    glfwSwapInterval(0);

    //-------------------CAMERA----------------------------//
    CameraSetup();
    //--------------------Selection Mode Stuff---------------------//
    BufferSetup();
}

void Engine::framebuffer_size_callback(GLFWwindow *WINDOW, int WIDTH, int HEIGHT) {
    Engine *engineInstance =
        static_cast<Engine *>(glfwGetWindowUserPointer(WINDOW));
    if (engineInstance) {
        glViewport(0, 0, WIDTH, HEIGHT);
    }
}

void Engine::mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    Engine *engineInstance =
        static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (engineInstance && engineInstance->CURSOR_HIDDEN) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (engineInstance->firstMouse) {
            engineInstance->lastX = xpos;
            engineInstance->lastY = ypos;
            engineInstance->firstMouse = false;
        }
        float xOffset = xpos - engineInstance->lastX;
        float yOffset = engineInstance->lastY - ypos;

        engineInstance->lastX = xpos;
        engineInstance->lastY = ypos;
        engineInstance->MainCamera->ProcessMouseMovement(xOffset, yOffset);
    }
}

void Engine::mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    Engine* engineInstance = static_cast<Engine*>(glfwGetWindowUserPointer(window));

    if(button == GLFW_MOUSE_BUTTON_LEFT){
        if(action == GLFW_PRESS){
            engineInstance->leftMousePressed = true;
        }else if(action == GLFW_RELEASE){
            engineInstance->leftMousePressed = false;
        }
    }
}

bool Engine::initGLFW() {
    if (!glfwInit()) {
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    return true;
    ;
}

bool Engine::initGLEW() {
    glewExperimental = GL_TRUE;
    return glewInit() == GLEW_OK;
}

bool Engine::isRunning() {
    return !glfwWindowShouldClose(window);
}

void Engine::CameraSetup() {
    MainCamera = new Camera(glm::vec3(0.0f, -0.5f, 4.0f));
    MainCamera->Projection = glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 1000.0f);

    lastX = width / 2.0f;
    lastY = height / 2.0f;
    firstMouse = true;

    deltaTime = 0.0f;
    lastFrame = 0.0f;
}

void Engine::DeltaTimeCalculation() {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

void Engine::ProcessMovement(GLFWwindow *window) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // THIS IS THE CODE FOR MOUSE TOGGLE
    bool tabPressed = glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS;
    if (tabPressed && !TAB_WAS_PRESSED) {
        if (CURSOR_HIDDEN) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        CURSOR_HIDDEN = !CURSOR_HIDDEN;
    }
    TAB_WAS_PRESSED = tabPressed;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        MainCamera->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        MainCamera->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        MainCamera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        MainCamera->ProcessKeyboard(RIGHT, deltaTime);

}

void Engine::Update() {
    DeltaTimeCalculation();
    
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);

    // 1. handle the initial click
    if(leftMousePressed && !isDragging){
        PerformSelection();
        isDragging = true;
    }
    // 2. Handle the dragging here
    if(leftMousePressed && isDragging && selectedModelID != 0){
        glm::vec3 newWorldPos = getWorldPosFromMouse(xPos, yPos, dragDepth);
        glm::vec3 targetPos = newWorldPos + grabOffset;

        float dragSpeed = 3000.0f * deltaTime;

        dragSpeed = glm::clamp(dragSpeed, 0.0f, 1.0f);

        glm::vec3& currentPos = sceneMgr.transforms[selectedModelID].pos;
        currentPos = currentPos + (targetPos - currentPos) * dragSpeed;
    }
    // Handle the release
    if(!leftMousePressed){
        isDragging = false;
    }

    // Clear the screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    view = MainCamera->GetViewMatrix();
    // Process Input
    ProcessMovement(window);
    // Update ECS Matrices
    // This updates the model matrices for all entities based on their Transform component
    sceneMgr.recalculateMatrix();
    // Draw the Scene
    renderer.draw(sceneMgr, resourceMgr, *MainCamera, *OutlineShader, *shadowShader, *directionalShadowMap, width, height);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Engine::BufferSetup() {
    // Directional Shadow Mapping
    directionalShadowMap = new ShadowMapFBO();
    shadowShader = new Shader("include/Essentials/ImpShaders/depthShader.vs", "include/Essentials/ImpShaders/depthShader.fs");
    // Skybox Setup
    std::vector<std::string> dayFaces = {
        "skybox/skybox_2/right.bmp",  
        "skybox/skybox_2/left.bmp",   
        "skybox/skybox_2/top.bmp",    
        "skybox/skybox_2/bottom.bmp", 
        "skybox/skybox_2/back.bmp",   
        "skybox/skybox_2/front.bmp"
    };
    unsigned int daySkyID = resourceMgr.loadCubemap("DaySky", dayFaces);
    sceneMgr.sky.shaderID = resourceMgr.loadShader("SkyShader", "include/Essentials/Skybox.vs", "include/Essentials/Skybox.fs");
    sceneMgr.sky.enabled = true;
    sceneMgr.sky.cubemapID = daySkyID;
    sceneMgr.sky.VAO = generateSkyboxVAO();
    
    // Pixel Picker
    picker = new PixelPicker(width, height);
    unsigned int defaultShader = resourceMgr.loadShader("default", "include/Essentials/ImpShaders/defaultShader.vs", "include/Essentials/ImpShaders/defaultShader.fs");
    pixelPickingShaderID = resourceMgr.loadShader("pixelPicker", "include/Essentials/ImpShaders/pixelPicker.vs", "include/Essentials/ImpShaders/pixelPicker.fs");
    // Adding Entity
    testEntityID = sceneMgr.addEntity("models/cyborg/cyborg.obj", glm::vec3(0.0f, 0.0f, -3.0f), defaultShader);
    sceneMgr.transforms[testEntityID].scale = glm::vec3(1.0f);
    sceneMgr.transforms[testEntityID].pos = glm::vec3(0.0f, 0.0f, -3.0f);
    testEntity2 = sceneMgr.addEntity("models/FlatPlane/flatPlane.obj", glm::vec3(0.0f, 0.0f, -4.0f), defaultShader);
    sceneMgr.transforms[testEntity2].scale = glm::vec3(1.0f);
    sceneMgr.transforms[testEntity2].pos = glm::vec3(2.0f, 0.0f, 0.0f);

    // Testing out the parent setting function
    sceneMgr.setParent(testEntity2, testEntityID);

    // Outlining using stencil shader
    OutlineShader = new Shader("include/Essentials/ImpShaders/outline.vs", "include/Essentials/ImpShaders/outline.fs");
}

void Engine::PerformSelection(){
    // bind to the framebuffer
    picker->bind();

    //Clear it with all 0 rgba values
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get the picking shader from the resource mgr and draw the scene in flat / solid colors based on entityIDs
    Shader* pickingShader = resourceMgr.getShader(pixelPickingShaderID);
    renderer.pixelPickerRenderPass(sceneMgr, *pickingShader, *MainCamera);

    // Now getting the id from the color under the mouse click
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    uint32_t clickedEntityID = picker->readPixel(mouseX, mouseY, height);

    // Moving on to the rest of the shit
    if(clickedEntityID != 0 && sceneMgr.meshes.count(clickedEntityID)){
        std::cout << "CLICKED_ENTITY::ID=" << clickedEntityID << std::endl;
        if(clickedEntityID != selectedModelID){
            DeselectAll();
        }
        sceneMgr.meshes[clickedEntityID].isSelected = true;
        selectedModelID = clickedEntityID;
        // Reading into dragDepth (a float variable /  not an array) works here because I'm reading only the depth component 
        glReadPixels((int)mouseX, height - (int)mouseY, 1, 1, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, &dragDepth);
        // Calculate the world position of this pixel
        glm::vec3 clickWorldPos = getWorldPosFromMouse(mouseX, mouseY, dragDepth);
        // Now we calculate the grab offset / the offset from the object's center in the world space
        grabOffset = sceneMgr.transforms[selectedModelID].pos - clickWorldPos;
    }else{
        std::cout << "NOTHING_CLICKED" << std::endl;
        DeselectAll();
        selectedModelID = 0;
    }
    // unbind the pixel picker's framebuffer at the end to make sure that the stuff above this line works
    picker->unbind();
}

void Engine::DeselectAll(){
    for(auto& [id, mesh] : sceneMgr.meshes){
        mesh.isSelected = false;
    }
}

unsigned int Engine::generateSkyboxVAO(){
    
    unsigned int VAO, VBO;

    float vertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    return VAO;
}

glm::vec3 Engine::getWorldPosFromMouse(double mouseX, double mouseY, float depth){
    float windowX = static_cast<float>(mouseX);
    float windowY = static_cast<float>(height) - static_cast<float>(mouseY);

    glm::vec4 viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
    
    glm::vec3 screenPos(windowX, windowY, depth);
    glm::vec3 worldPos = glm::unProject(screenPos, MainCamera->GetViewMatrix(), MainCamera->GetProjectionMatrix(), viewport);

    return worldPos;
}