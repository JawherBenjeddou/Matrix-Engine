#version 460 core

layout (location = 0) in vec2 aPos;

uniform mat4 u_ViewProjection;
uniform mat4 u_model;

void main()
{
	gl_Position = vec4(aPos,0.0,1.0);
}