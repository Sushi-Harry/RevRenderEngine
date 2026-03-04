#version 330

out vec4 FragColor;

uniform vec3 PickingColor;

void main(){
    FragColor = vec4(PickingColor, 0.0f);
}
