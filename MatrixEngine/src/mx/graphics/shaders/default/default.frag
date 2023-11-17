#version 460 core
out vec4 FragColor;


uniform vec4 u_Color;
in vec2 SpriteTextureCoord;

uniform sampler2D u_SpriteTextureID;

void main()
{	
	FragColor = mix(u_Color,texture(u_SpriteTextureID,SpriteTextureCoord),0.5);
}