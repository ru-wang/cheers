#version 450

uniform mat4 mvp;
uniform float metric;

in vec3 pos;
in vec3 clr;

out vec3 clr_out;

void main() {
  gl_Position = mvp * vec4(metric * pos, 1.0f);
  clr_out = clr;
}
