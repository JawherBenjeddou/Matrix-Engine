#version 460 core
out vec4 FragColor;


uniform vec4 u_Color;
in vec2 SpriteTextureCoord;

uniform sampler2D u_SpriteTextureID;

void main()
{	
	//FragColor = mix(u_Color,texture(u_SpriteTextureID,SpriteTextureCoord),1.0);
	FragColor =	vec4(1.0f,0.0f,0.0f,1.0f);
}