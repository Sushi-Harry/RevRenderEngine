#include "PixelPicker.hpp"
#include <GL/glew.h>

PixelPicker::PixelPicker(int width, int height){
    initFBO(width, height);
}

void PixelPicker::bind(){
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void PixelPicker::unbind(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint32_t PixelPicker::readPixel(int x, int y, int windowHeight){
    bind();
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    unsigned char data[4];
    glReadPixels(x, windowHeight - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

    unbind();

    return data[0] + (data[1] << 8) + (data[2] << 16);
}

glm::vec3 PixelPicker::IDtoColor(uint32_t id){
    int r = (id & 0x000000FF) >> 0;
    int g = (id & 0x0000FF00) >> 8;
    int b = (id & 0x00FF0000) >> 16;
    
    return glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
}

uint32_t PixelPicker::ColorToID(unsigned int r, unsigned int g, unsigned int b){
    return r + (g << 8) + (b << 16);
}

// Boilerplate code for framebuffer initialization
void PixelPicker::initFBO(int width, int height){
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &pickingTexture);
    glBindTexture(GL_TEXTURE_2D, pickingTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,pickingTexture, 0);

    // Generating the data for the renderbuffer
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::SELECTION_FRAMEBUFFER_NOT_COMPLETE" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}