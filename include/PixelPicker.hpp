#include <iostream>
#include <glm/glm.hpp>

class PixelPicker{
public:
    PixelPicker(int width, int height);
    ~PixelPicker();

    void bind();
    void unbind();
    uint32_t readPixel(int x, int y, int windowHeight);

    void resize(int newWidth, int newHeight);

    glm::vec3 IDtoColor(uint32_t id);
    uint32_t ColorToID(unsigned int r, unsigned int g, unsigned int b);
private:
    unsigned int FBO;
    unsigned int pickingTexture;
    unsigned int depthBuffer;

    void initFBO(int width, int height);
};