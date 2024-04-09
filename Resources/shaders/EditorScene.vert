#version 420 core

#include "assets/shaders/Core.verth"

out vec2 TexCoords;

void main()
{
	mat4 mvp = sv_viewProjection * sv_modelMat;
	gl_Position = mvp * vec4(_pos, 1.0);
	TexCoords = _texCoords;
}