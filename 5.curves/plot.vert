#version 330

layout (location = 0) in vec2 a_position;

void main()
{
  gl_Position = vec4(a_position,0.f,1.f);
}
