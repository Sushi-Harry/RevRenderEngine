#include "Engine.hpp"
#include "ShaderClass.hpp"
#include "camera.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const double FIXED_TIME_STEP = 1.0 / 60.0;

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

    initIMGUI();
    
    // Physics Setup
    physicsWorld = physicsCommon.createPhysicsWorld();
    // Test that it worked by printing the default gravity
    rp3d::Vector3 gravity = physicsWorld->getGravity();
    std::cout << "Physics Engine Initialized! Gravity: " 
    << gravity.x << ", " << gravity.y << ", " << gravity.z << std::endl;
    
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

void Engine::initIMGUI(){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();   (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430 core");
}

void Engine::imgui_startFrame(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Engine::imgui_UI(){
    ImGui::Begin("Engine Controls");
    ImGui::Text("Performance: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    if (ImGui::CollapsingHeader("Physics Engine")) {
        // A simple checkbox to toggle your debug wireframes!
        static bool showDebug = true;
        if (ImGui::Checkbox("Show Debug Wireframes", &showDebug)) {
            physicsWorld->setIsDebugRenderingEnabled(showDebug);
        }
    }
    ImGui::End();

    //================================================//
    //========ENTITY INSPECTOR========================//
    //================================================//
    ImGui::Begin("Entity Inspector");
    ImGui::Text("Selected Entity: ID %d", selectedModelID);
    ImGui::Separator();

    // Graphical Properties
    if(sceneMgr.transforms.count(selectedModelID)){
        Transform& t = sceneMgr.transforms[selectedModelID];
        ImGui::Text("GRAPHIC TRANSFORMATIONS");
        ImGui::DragFloat3("Scale", glm::value_ptr(t.scale), 0.05f, 0.1f, 100.0f);
    }
    // Physics Component Properties
    if(sceneMgr.rigidBodies.count(selectedModelID)){
        ImGui::Separator();
        ImGui::Text("Physics Properties");

        rp3d::RigidBody* body = sceneMgr.rigidBodies[selectedModelID].body;
        if(body){
            rp3d::Transform currentTransform = body->getTransform();
            rp3d::Vector3 pos = currentTransform.getPosition();

            float posArray[3] = {
                static_cast<float>(pos.x),
                static_cast<float>(pos.y),
                static_cast<float>(pos.z)
            };
            if(ImGui::DragFloat3("Position", posArray, 0.1f)){
                currentTransform.setPosition(rp3d::Vector3(posArray[0], posArray[1], posArray[2]));
                body->setTransform(currentTransform);
                body->setIsActive(true);
            }

            // boounciness 
            if(body->getNbColliders() > 0){
                rp3d::Collider* collider = body->getCollider(0);
                rp3d::Material& material = collider->getMaterial();

                float bounciness = material.getBounciness();
                if(ImGui::SliderFloat("Bounciness", &bounciness, 0.0f, 1.0f)){
                    material.setBounciness(bounciness);
                }
            }
        }
    }else{
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No entity selected.");
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Click an object in the scene!");
    }
    ImGui::End();

    //================================================//
    //========CONTENT BROWSER=========================//
    //================================================//
    ImGui::Begin("Content Browser");
    static char spawnPath[256] = "";
    static float spawnLoc[3] = {0.0f, 0.0f, 0.0f};
    static int physicsType = 1; // 0 = none, 1 = Dynamic box, 2 = static mesh

    ImGui::Text("Spawn New Entity");
    ImGui::Separator();
    ImGui::InputText("Model Path", spawnPath, 256);
    ImGui::DragFloat3("Spawn Position", spawnLoc, 0.1f);
    ImGui::Text("Physics Body Type");
    ImGui::RadioButton("None", &physicsType, 0); ImGui::SameLine();
    ImGui::RadioButton("Dynamic", &physicsType, 1); ImGui::SameLine();
    ImGui::RadioButton("Static", &physicsType, 2);

    ImGui::Spacing();

    if(ImGui::Button("ADD ENTITY", ImVec2(-1.0, 30.0))){
        unsigned int shaderID = resourceMgr.loadShader("default", "include/Essentials/ImpShaders/defaultShader.vs", "include/Essentials/ImpShaders/defaultShader.fs");

        // Add the entitty
        glm::vec3 pos(spawnLoc[0], spawnLoc[1], spawnLoc[2]);
        uint32_t newID = sceneMgr.addEntity(spawnPath, pos, shaderID, resourceMgr);
        sceneMgr.transforms[newID].scale = glm::vec3(1.0f);
        sceneMgr.transforms[newID].pos = pos;

        if(physicsType > 0){
            rp3d::Transform startTransform(rp3d::Vector3(pos.x, pos.y, pos.z), rp3d::Quaternion::identity());
            rp3d::RigidBody* body = physicsWorld->createRigidBody(startTransform);
            body->setIsDebugEnabled(true);

            if(physicsType == 1){
                body->setType(rp3d::BodyType::DYNAMIC);
                rp3d::BoxShape* boxShape = physicsCommon.createBoxShape(rp3d::Vector3(1.0, 1.0, 1.0));
                body->addCollider(boxShape, rp3d::Transform::identity());
            }else if(physicsType == 2){
                body->setType(rp3d::BodyType::KINEMATIC);
                const auto& verts = resourceMgr.getVertices(spawnPath);
                const auto& inds = resourceMgr.getIndices(spawnPath);
                
                if (!verts.empty() && !inds.empty()) {
                    rp3d::TriangleVertexArray* triArray = new rp3d::TriangleVertexArray(
                        verts.size(), &(verts[0].position.x), sizeof(VertexComponent),
                        inds.size() / 3, inds.data(), 3 * sizeof(uint32_t),
                        rp3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
                        rp3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE
                    );
                    
                    std::vector<rp3d::Message> messages;
                    rp3d::TriangleMesh* phyMesh = physicsCommon.createTriangleMesh(*triArray, messages);
                    rp3d::ConcaveMeshShape* concaveMesh = physicsCommon.createConcaveMeshShape(phyMesh);
                    body->addCollider(concaveMesh, rp3d::Transform::identity());
                } else {
                    std::cout << "[WARNING] Could not load physics mesh for " << spawnPath << std::endl;
                }
            }

            sceneMgr.rigidBodies[newID] = RigidBodyComponent{body};
        }

        DeselectAll();
        sceneMgr.meshes[newID].isSelected = true;   
        selectedModelID = newID;
    }
    ImGui::End();
}

void Engine::imgui_endFrame(){
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

void Engine::debug_render_physics(){
    if(!physicsWorld->getIsDebugRenderingEnabled()){
        return;
    }

    // get the renderer first
    rp3d::DebugRenderer& debugRenderer = physicsWorld->getDebugRenderer();
    debugRenderer.computeDebugRenderingPrimitives(*physicsWorld);
    // get the number of lines
    uint32_t numLines = debugRenderer.getNbLines();
    uint32_t numTriangles = debugRenderer.getNbTriangles();

    if(numLines > 0 || numTriangles > 0){
        std::vector<float> lineVertices;
        // Opengl uses float but rp3d uses double so yk, we gotta do some ricebag conversion type shit
        lineVertices.reserve((numLines * 6) + (numTriangles * 9)); // Reserve the space first. 2 points per line. 3 floats per point. So 3*2 = 6
        
        //==========================EXTRACTING THE LINE DATA==================================//
        if(numLines > 0){
            const rp3d::DebugRenderer::DebugLine* lines = debugRenderer.getLinesArray();
            for(uint32_t i = 0; i < numLines; i++){
                lineVertices.push_back(static_cast<float>(lines[i].point1.x));
                lineVertices.push_back(static_cast<float>(lines[i].point1.y));
                lineVertices.push_back(static_cast<float>(lines[i].point1.z));

                lineVertices.push_back(static_cast<float>(lines[i].point2.x));
                lineVertices.push_back(static_cast<float>(lines[i].point2.y));
                lineVertices.push_back(static_cast<float>(lines[i].point2.z));
            }
        }
        //==========================EXTRACTING THE TRIANGLES DATA==================================//
        if(numTriangles > 0){
            const rp3d::DebugRenderer::DebugTriangle* triangles = debugRenderer.getTrianglesArray();
            for(uint32_t i = 0; i < numTriangles; i++){
                // Point 1
                lineVertices.push_back(static_cast<float>(triangles[i].point1.x));
                lineVertices.push_back(static_cast<float>(triangles[i].point1.y));
                lineVertices.push_back(static_cast<float>(triangles[i].point1.z));

                lineVertices.push_back(static_cast<float>(triangles[i].point2.x));
                lineVertices.push_back(static_cast<float>(triangles[i].point2.y));
                lineVertices.push_back(static_cast<float>(triangles[i].point2.z));

                lineVertices.push_back(static_cast<float>(triangles[i].point3.x));
                lineVertices.push_back(static_cast<float>(triangles[i].point3.y));
                lineVertices.push_back(static_cast<float>(triangles[i].point3.z));
            }
        }

        // now we hand it over to the gpu
        glBindVertexArray(debugVAO);
        glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
        glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(float), lineVertices.data(), GL_DYNAMIC_DRAW);

        Shader* debugShader = resourceMgr.getShader(debugShaderID);
        debugShader->use();
        debugShader->setMat4("view", MainCamera->GetViewMatrix());
        debugShader->setMat4("projection", MainCamera->GetProjectionMatrix());

        // Turn off depth testing so lines draw OVER the solid meshes
        glDisable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        if(numLines > 0){
            glDrawArrays(GL_LINES, 0, numLines * 2);
        }
        if(numTriangles > 0){
            glDrawArrays(GL_TRIANGLES, numLines*2, numTriangles*3);
        }
        
        // Turn it back on for the next frame
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_DEPTH_TEST);
        glBindVertexArray(0);
    }
    // std::cout << "Lines: " << numLines << " | Triangles: " << numTriangles << std::endl;
}

void Engine::Update() {
    DeltaTimeCalculation();
    // This is the accumulator loop. This part of the code makes the physics engine independent of the renderer's framerate
    accumulator += deltaTime;
    while(accumulator >= FIXED_TIME_STEP){
        physicsWorld->update(FIXED_TIME_STEP);
        accumulator -= FIXED_TIME_STEP;
    }
    SyncPhysicsToGraphics();
    
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    
    // 1. handle the initial click
    ImGuiIO& io = ImGui::GetIO();
    if(leftMousePressed && !isDragging){
        if(!io.WantCaptureMouse){
            PerformSelection();
            isDragging = true;
        }
    }
    // 2. Handle the dragging here
    if(leftMousePressed && isDragging && selectedModelID != 0){
        glm::vec3 newWorldPos = getWorldPosFromMouse(xPos, yPos, dragDepth);
        glm::vec3 targetPos = newWorldPos + grabOffset;

        float dragSpeed = 1.0;

        dragSpeed = glm::clamp(dragSpeed, 0.0f, 1.0f);

        glm::vec3& currentPos = sceneMgr.transforms[selectedModelID].pos;
        currentPos = currentPos + (targetPos - currentPos) * dragSpeed;

        // Making it match with the physics world
        if(sceneMgr.rigidBodies.count(selectedModelID)){
            rp3d::RigidBody* body = sceneMgr.rigidBodies[selectedModelID].body;
            if(body){
                rp3d::Transform currentPhyTransform = body->getTransform();
                
                // Update the position based on the currntPos vector
                currentPhyTransform.setPosition(rp3d::Vector3(currentPos.x, currentPos.y, currentPos.z));
                body->setTransform(currentPhyTransform);

                // Reset the velocity too
                body->setLinearVelocity(rp3d::Vector3(0.0, 0.0, 0.0));
                body->setAngularVelocity(rp3d::Vector3(0.0, 0.0, 0.0));
                
            }
        }
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

    // Debug Renderer
    debug_render_physics();

    // Get the GUI Working
    imgui_startFrame();
    imgui_UI();
    imgui_endFrame();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Engine::debug_init_physics(){
    glGenVertexArrays(1, &debugVAO);
    glGenBuffers(1, &debugVBO);
    glBindVertexArray(debugVAO);
    glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
    // We use GL_DYNAMIC_DRAW because the lines change every frame!
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // 3. Turn on the Physics Debugger
    physicsWorld->setIsDebugRenderingEnabled(true);
    rp3d::DebugRenderer& debugRenderer = physicsWorld->getDebugRenderer();

    // Tell it exactly what you want to see (i.e., the collision shapes)
    debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
    debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_AABB, false);
}

void Engine::BufferSetup() {
    // debuguguguguguguggingg
    debugShaderID = resourceMgr.loadShader("debugPhysics", "include/Essentials/debug/physics.debug.vs", "include/Essentials/debug/physics.debug.fs");
    debug_init_physics();
    // Enable debug rendering
    physicsWorld->setIsDebugRenderingEnabled(true);

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
    
    // Physics Setup
    // This is the test subject - Cyborg
    std::string cyborgPath = "models/cyborg/cyborg.obj";
    testEntityID = sceneMgr.addEntity(cyborgPath, glm::vec3(0.0f, 5.0f, -3.0f), defaultShader, resourceMgr);
    sceneMgr.transforms[testEntityID].scale = glm::vec3(1.0f);
    sceneMgr.transforms[testEntityID].pos = glm::vec3(0.0f, 5.0f, -3.0f);
    
    rp3d::Transform cyTransform(rp3d::Vector3(0.0f, 5.0f, -3.0f), rp3d::Quaternion::identity());
    rp3d::RigidBody* cybody = physicsWorld->createRigidBody(cyTransform);
    cybody->setType(rp3d::BodyType::DYNAMIC);
    cybody->setIsDebugEnabled(true);

    // 1. DYNAMIC BODIES MUST USE CONVEX SHAPES (Box, Sphere, Capsule)
    rp3d::BoxShape* cyborgBox = physicsCommon.createBoxShape(rp3d::Vector3(1.0, 2.0, 0.3));
    
    // 2. You can shift the box so it aligns with the visual mesh
    rp3d::Transform localOffset(rp3d::Vector3(0.0, 2.0, 0.0), rp3d::Quaternion::identity());
    
    rp3d::Collider* cyCollider = cybody->addCollider(cyborgBox, localOffset);
    rp3d::Material& material = cyCollider->getMaterial();
    material.setBounciness(0.5);
    
    sceneMgr.rigidBodies[testEntityID] = RigidBodyComponent{ cybody };

    // This acts as the floor - Terrain model - Mesh Accurate
    std::string terrainPath = "models/ground_flat/flat_plane.obj";
    // 1. Move the floor to Z = -3.0 so it sits perfectly beneath the cyborg
    testEntity2 = sceneMgr.addEntity(terrainPath.c_str(), glm::vec3(0.0f, -2.0f, -3.0f), defaultShader, resourceMgr);
    // 2. Make it visually massive
    sceneMgr.transforms[testEntity2].scale = glm::vec3(1.0f, 1.0f, 1.0f);
    sceneMgr.transforms[testEntity2].pos = glm::vec3(0.0f, -2.0f, -3.0f);
    // 3. Match the physics starting position
    rp3d::Transform floorTransform(rp3d::Vector3(0.0, -2.0, -3.0), rp3d::Quaternion::identity());
    rp3d::RigidBody* floorBody = physicsWorld->createRigidBody(floorTransform);
    const auto& cpuVertices = resourceMgr.getVertices(terrainPath);
    const auto& cpuIndices = resourceMgr.getIndices(terrainPath);
    if (!cpuVertices.empty() && !cpuIndices.empty()) {
        rp3d::TriangleVertexArray* triangleArray = new rp3d::TriangleVertexArray(
            cpuVertices.size(),
            &(cpuVertices[0].position.x),
            sizeof(VertexComponent),
            cpuIndices.size() / 3,
            cpuIndices.data(),
            3*sizeof(uint32_t),
            rp3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
            rp3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE
        );
        
        std::vector<rp3d::Message> messages;
        rp3d::TriangleMesh* phyMesh = physicsCommon.createTriangleMesh(*triangleArray, messages);
        rp3d::ConcaveMeshShape* concaveMesh = physicsCommon.createConcaveMeshShape(phyMesh);

        // 4. CRITICAL FIX: Tell the physics engine to stretch the mesh to match your graphics scale!
        concaveMesh->setScale(rp3d::Vector3(1.0, 1.0, 1.0));

        // attach the collider
        floorBody->addCollider(concaveMesh, rp3d::Transform::identity());
        floorBody->setType(rp3d::BodyType::KINEMATIC);
        floorBody->setIsDebugEnabled(true);
        sceneMgr.rigidBodies[testEntity2] = RigidBodyComponent{ floorBody };
    }

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

void Engine::SyncPhysicsToGraphics(){
    for(auto& [id, rb] : sceneMgr.rigidBodies){
        if(sceneMgr.transforms.count(id) && rb.body != nullptr){
            Transform& t = sceneMgr.transforms[id];
            const rp3d::Transform& phyTransform = rb.body->getTransform();

            // First, we sync the position
            const rp3d::Vector3& pos = phyTransform.getPosition();
            t.pos = glm::vec3(pos.x, pos.y, pos.z);

            // Get the 4x4 matrix - This shit is a pain in the ass cause I decided to use the AUR packages that expects doubles instead of floats EVERYWHERE
            rp3d::decimal glMat[16];
            phyTransform.getOpenGLMatrix(glMat);

            // Convert this matrix to glm matrix
            glm::mat4 phyMat;
            float* matPtr = glm::value_ptr(phyMat);
            for(int i = 0; i < 16; i++){
                matPtr[i] = static_cast<float>(glMat[i]);
            }

            // Just apply the scale to the object since rp3d handles translation and rotation
            t.modelMatrix = glm::scale(phyMat, t.scale);
            
        }
    }
}