#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

in vec4 v_Color;
in vec2 v_TexCoord;
in flat float v_TexIndex;
in vec2 v_Tiling;
in flat int v_EntityID;

uniform sampler2D u_Textures[16];

void main()
{
	vec4 texColor = v_Color;
	vec2 uv = v_TexCoord * v_Tiling;
	switch (int(v_TexIndex))
	{
		case  0: texColor *= texture(u_Textures[ 0], uv); break;
		case  1: texColor *= texture(u_Textures[ 1], uv); break;
		case  2: texColor *= texture(u_Textures[ 2], uv); break;
		case  3: texColor *= texture(u_Textures[ 3], uv); break;
		case  4: texColor *= texture(u_Textures[ 4], uv); break;
		case  5: texColor *= texture(u_Textures[ 5], uv); break;
		case  6: texColor *= texture(u_Textures[ 6], uv); break;
		case  7: texColor *= texture(u_Textures[ 7], uv); break;
		case  8: texColor *= texture(u_Textures[ 8], uv); break;
		case  9: texColor *= texture(u_Textures[ 9], uv); break;
		case 10: texColor *= texture(u_Textures[10], uv); break;
		case 11: texColor *= texture(u_Textures[11], uv); break;
		case 12: texColor *= texture(u_Textures[12], uv); break;
		case 13: texColor *= texture(u_Textures[13], uv); break;
		case 14: texColor *= texture(u_Textures[14], uv); break;
		case 15: texColor *= texture(u_Textures[15], uv); break;
	}

	if (texColor.a < 0.01)
		discard;

	o_Color = texColor;
	o_EntityID = v_EntityID;
}
