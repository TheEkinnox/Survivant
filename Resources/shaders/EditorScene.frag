#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform vec2		u_entityID;

void main()
{
	FragColor = vec4(u_entityID, 1, 1);
}