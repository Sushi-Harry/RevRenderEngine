# Details

Date : 2026-03-04 20:19:02

Directory /home/harry/Projects/RevRenderEngine

Total : 55 files,  2588 codes, 280 comments, 609 blanks, all 3477 lines

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [BASE\_SHADERS/LIT\_BASE\_SHADER.fs](/BASE_SHADERS/LIT_BASE_SHADER.fs) | GLSL | 99 | 15 | 33 | 147 |
| [BASE\_SHADERS/LIT\_BASE\_SHADER.vs](/BASE_SHADERS/LIT_BASE_SHADER.vs) | GLSL | 17 | 0 | 4 | 21 |
| [MiscTools/createLitShader.sh](/MiscTools/createLitShader.sh) | Shell Script | 11 | 1 | 5 | 17 |
| [MiscTools/normalizeColorCoordinates.sh](/MiscTools/normalizeColorCoordinates.sh) | Shell Script | 10 | 1 | 5 | 16 |
| [README.md](/README.md) | Markdown | 18 | 0 | 4 | 22 |
| [compile\_commands.json](/compile_commands.json) | JSON | 210 | 0 | 1 | 211 |
| [imgui.ini](/imgui.ini) | Ini | 9 | 0 | 4 | 13 |
| [include/Components.hpp](/include/Components.hpp) | C++ | 74 | 8 | 17 | 99 |
| [include/Engine.cpp](/include/Engine.cpp) | C++ | 273 | 32 | 62 | 367 |
| [include/Engine.hpp](/include/Engine.hpp) | C++ | 56 | 7 | 11 | 74 |
| [include/Essentials/ImpShaders/defaultShader.fs](/include/Essentials/ImpShaders/defaultShader.fs) | GLSL | 119 | 21 | 39 | 179 |
| [include/Essentials/ImpShaders/defaultShader.vs](/include/Essentials/ImpShaders/defaultShader.vs) | GLSL | 20 | 0 | 4 | 24 |
| [include/Essentials/ImpShaders/depthShader.fs](/include/Essentials/ImpShaders/depthShader.fs) | GLSL | 4 | 1 | 1 | 6 |
| [include/Essentials/ImpShaders/depthShader.vs](/include/Essentials/ImpShaders/depthShader.vs) | GLSL | 7 | 0 | 2 | 9 |
| [include/Essentials/ImpShaders/outline.fs](/include/Essentials/ImpShaders/outline.fs) | GLSL | 5 | 0 | 0 | 5 |
| [include/Essentials/ImpShaders/outline.vs](/include/Essentials/ImpShaders/outline.vs) | GLSL | 8 | 0 | 0 | 8 |
| [include/Essentials/ImpShaders/pixelPicker.fs](/include/Essentials/ImpShaders/pixelPicker.fs) | GLSL | 6 | 0 | 4 | 10 |
| [include/Essentials/ImpShaders/pixelPicker.vs](/include/Essentials/ImpShaders/pixelPicker.vs) | GLSL | 8 | 0 | 3 | 11 |
| [include/Essentials/ImpShaders/stencilBuffer.fs](/include/Essentials/ImpShaders/stencilBuffer.fs) | GLSL | 5 | 0 | 3 | 8 |
| [include/Essentials/PostprocessingShaders/colorInversion.fs](/include/Essentials/PostprocessingShaders/colorInversion.fs) | GLSL | 7 | 0 | 4 | 11 |
| [include/Essentials/PostprocessingShaders/gaussianBlur.fs](/include/Essentials/PostprocessingShaders/gaussianBlur.fs) | GLSL | 32 | 0 | 8 | 40 |
| [include/Essentials/PostprocessingShaders/grayscale.fs](/include/Essentials/PostprocessingShaders/grayscale.fs) | GLSL | 9 | 0 | 6 | 15 |
| [include/Essentials/PostprocessingShaders/screenQuad.vs](/include/Essentials/PostprocessingShaders/screenQuad.vs) | GLSL | 8 | 0 | 3 | 11 |
| [include/Essentials/PostprocessingShaders/sharpen.fs](/include/Essentials/PostprocessingShaders/sharpen.fs) | GLSL | 32 | 0 | 8 | 40 |
| [include/Essentials/Skybox.fs](/include/Essentials/Skybox.fs) | GLSL | 7 | 0 | 4 | 11 |
| [include/Essentials/Skybox.vs](/include/Essentials/Skybox.vs) | GLSL | 12 | 1 | 3 | 16 |
| [include/Essentials/grid.fs](/include/Essentials/grid.fs) | GLSL | 21 | 7 | 12 | 40 |
| [include/Essentials/grid.vs](/include/Essentials/grid.vs) | GLSL | 11 | 0 | 4 | 15 |
| [include/Framebuffer\_Shadowmap.cpp](/include/Framebuffer_Shadowmap.cpp) | C++ | 17 | 1 | 4 | 22 |
| [include/Framebuffer\_Shadowmap.hpp](/include/Framebuffer_Shadowmap.hpp) | C++ | 12 | 0 | 5 | 17 |
| [include/Grid.cpp](/include/Grid.cpp) | C++ | 46 | 1 | 12 | 59 |
| [include/Grid.hpp](/include/Grid.hpp) | C++ | 12 | 0 | 3 | 15 |
| [include/ModelLoader.cpp](/include/ModelLoader.cpp) | C++ | 85 | 14 | 19 | 118 |
| [include/ModelLoader.hpp](/include/ModelLoader.hpp) | C++ | 14 | 0 | 5 | 19 |
| [include/PixelPicker.cpp](/include/PixelPicker.cpp) | C++ | 46 | 2 | 16 | 64 |
| [include/PixelPicker.hpp](/include/PixelPicker.hpp) | C++ | 18 | 0 | 5 | 23 |
| [include/RenderSystem.hpp](/include/RenderSystem.hpp) | C++ | 180 | 26 | 33 | 239 |
| [include/ResourceManager.hpp](/include/ResourceManager.hpp) | C++ | 71 | 2 | 13 | 86 |
| [include/SceneManager.hpp](/include/SceneManager.hpp) | C++ | 58 | 30 | 16 | 104 |
| [include/ShaderClass.hpp](/include/ShaderClass.hpp) | C++ | 157 | 29 | 9 | 195 |
| [include/ShadowMap.hpp](/include/ShadowMap.hpp) | C++ | 47 | 1 | 13 | 61 |
| [include/Skybox.cpp](/include/Skybox.cpp) | C++ | 115 | 1 | 21 | 137 |
| [include/Skybox.hpp](/include/Skybox.hpp) | C++ | 18 | 0 | 7 | 25 |
| [include/TextureLoader.hpp](/include/TextureLoader.hpp) | C++ | 64 | 5 | 15 | 84 |
| [include/camera.h](/include/camera.h) | C++ | 101 | 20 | 17 | 138 |
| [include/lights.cpp](/include/lights.cpp) | C++ | 83 | 5 | 20 | 108 |
| [include/lights.hpp](/include/lights.hpp) | C++ | 61 | 8 | 28 | 97 |
| [main.cpp](/main.cpp) | C++ | 8 | 0 | 1 | 9 |
| [makefile](/makefile) | Makefile | 23 | 10 | 11 | 44 |
| [shaders/cyborgShader.fs](/shaders/cyborgShader.fs) | GLSL | 99 | 15 | 33 | 147 |
| [shaders/cyborgShader.vs](/shaders/cyborgShader.vs) | GLSL | 17 | 0 | 4 | 21 |
| [shaders/flatSurface.fs](/shaders/flatSurface.fs) | GLSL | 99 | 15 | 33 | 147 |
| [shaders/flatSurface.vs](/shaders/flatSurface.vs) | GLSL | 16 | 0 | 5 | 21 |
| [shaders/sun.fs](/shaders/sun.fs) | GLSL | 10 | 1 | 4 | 15 |
| [shaders/sun.vs](/shaders/sun.vs) | GLSL | 13 | 0 | 3 | 16 |

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)