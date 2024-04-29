#version 330 core

layout(location = 0) out int FragId;

uniform int u_entityID;

void main()
{
	FragId = u_entityID;
}