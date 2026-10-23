#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

in vec4 v_Color;
in flat int v_EntityID;

void main()
{
	o_Color = v_Color;
	o_EntityID = v_EntityID;
}
