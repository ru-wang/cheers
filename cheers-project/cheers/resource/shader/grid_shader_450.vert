#version 450

uniform mat4 mvp;
uniform float metric;

in ivec3 pos;

void main() {
  gl_Position = mvp * vec4(metric * pos, 1.0f);
}
