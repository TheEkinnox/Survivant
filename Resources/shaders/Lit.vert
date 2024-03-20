#version 420 core

#include "assets/shaders/Core.verth"

out VS_OUT
{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} vs_out;

void main()
{
	mat4 mvp = sv_viewProjection * sv_modelMat;
	gl_Position = mvp * vec4(_pos, 1.0);

	vs_out.FragPos = (sv_modelMat * vec4(_pos, 1.0)).xyz;
	vs_out.Normal = (sv_normalMat * vec4(_normal, 0)).xyz;
	vs_out.TexCoords = _texCoords;
}