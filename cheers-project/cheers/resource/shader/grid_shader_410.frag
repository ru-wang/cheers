#version 410

uniform vec3 clr;

layout (location = 0) out vec4 Out_Color;

void main() {
  Out_Color = vec4(clr, 1.0f);
}
