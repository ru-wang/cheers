#version 460

in vec3 clr_out;

layout (location = 0) out vec4 Out_Color;

void main() {
  Out_Color = vec4(clr_out, 1.0f);
}
