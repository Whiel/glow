#version 330

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;

uniform mat4 u_mvp;
uniform mat3 u_normal_mat;

out vec3 v_normal;
out vec3 v_pos;

void main()
{
  gl_Position = u_mvp*vec4(a_position,1.f);
  v_pos = gl_Position.xyz / gl_Position.w;
  v_normal = u_normal_mat*a_normal;
}
