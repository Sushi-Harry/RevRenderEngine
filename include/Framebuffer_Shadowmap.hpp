#pragma once
#include <GL/glew.h>

class ShadowMap_Framebuffer{

public:
    const int SHADOW_HEIGHT = 1024;
    const int SHADOW_WIDTH = 1024;

    ShadowMap_Framebuffer();
private:
    unsigned int depthMapFBO;
    unsigned int depthMap;

    void SETUP_DEPTHMAP();

};