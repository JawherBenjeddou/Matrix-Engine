#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 uv;


uniform mat4 u_ViewProjection;
uniform mat4 u_model;

out vec2 SpriteTextureCoord;

void main()
{
	gl_Position = vec4(aPos,0.0,1.0);
	SpriteTextureCoord = uv;
}


	