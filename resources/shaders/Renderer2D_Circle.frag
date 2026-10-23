#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

in vec3 v_LocalPosition;
in vec4 v_Color;
in float v_Thickness;
in float v_Fade;
in flat int v_EntityID;

void main()
{
	// Signed distance from the circle edge, in local [-1, 1] space.
	float distance = 1.0 - length(v_LocalPosition.xy);
	float circle = smoothstep(0.0, v_Fade, distance);
	circle *= smoothstep(v_Thickness + v_Fade, v_Thickness, distance);

	if (circle <= 0.0)
		discard;

	o_Color = v_Color;
	o_Color.a *= circle;
	o_EntityID = v_EntityID;
}
