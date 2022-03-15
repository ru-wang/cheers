#version 460

uniform vec4 clr;

layout (location = 0) out vec4 Out_Color;

void main() {
  Out_Color = clr;
}
