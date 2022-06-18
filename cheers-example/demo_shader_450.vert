#version 450

in vec3 vertex;
in vec3 color;
in vec4 rotation_c0;
in vec4 rotation_c1;
in vec4 rotation_c2;
in vec4 translation;
out vec3 vertex_color;

uniform mat4 proj_matrix;
uniform mat4 mv_matrix;
uniform mat4 extrinsics;

void main() {
  mat4 pose = mat4(rotation_c0, rotation_c1, rotation_c2, translation);
  gl_Position = proj_matrix * mv_matrix * pose * extrinsics * vec4(vertex, 1.0f);
  vertex_color = color;
}
