#version 430 core

#include "assets/shaders/Core.fragh"
#include "assets/shaders/BlinnPhong.glslh"
#include "assets/shaders/Light.glslh"

in VS_OUT
{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

out vec4 FragColor;

uniform vec4      u_tint;
uniform vec4      u_specularColor;
uniform float     u_shininess;
uniform sampler2D u_diffuse;

vec3 calculateLight(mat4 p_lightMat);
vec3 calculateAmbientLight(vec4 p_color);
vec3 calculateDirLight(DirLight p_light);
vec3 calculatePointLight(PointLight p_light);
vec3 calculateSpotLight(SpotLight p_light);

vec4 g_diffColor;
vec3 g_normal;
vec3 g_viewDir;

void main()
{
	g_diffColor = texture(u_diffuse, fs_in.TexCoords) * u_tint;
	g_normal = normalize(fs_in.Normal);
	g_viewDir = normalize(sv_viewPos - fs_in.FragPos);

	vec3 litColor = vec3(0);

	for (int i = 0; i < ssbo_lights.length(); ++i)
		litColor += calculateLight(ssbo_lights[i]);

	FragColor = vec4(litColor, g_diffColor.a);
}

vec3 calculateLight(mat4 p_lightMat)
{
	switch (int(p_lightMat[3][3]))
	{
	case 0:
		return calculateAmbientLight(getColor(p_lightMat));
	case 1:
		return calculateDirLight(getDirLight(p_lightMat));
	case 2:
		return calculatePointLight(getPointLight(p_lightMat));
	case 3:
		return calculateSpotLight(getSpotLight(p_lightMat));
	default:
		return vec3(0);
	}
}

vec3 calculateAmbientLight(vec4 color)
{
	return color.rgb * color.a * g_diffColor.rgb;
}

vec3 calculateDirLight(DirLight p_light)
{
	vec3 lightDir = normalize(-p_light.direction);

	BlinnPhongData data =
	{
		lightDir,
		g_viewDir,
		p_light.color,
		g_diffColor,
		u_specularColor,
		g_normal,
		u_shininess
	};

	return computeBlinnPhong(data);
}

vec3 calculatePointLight(PointLight p_light)
{
	vec3 lightDir = normalize(p_light.position - fs_in.FragPos);

	float distance = length(p_light.position - fs_in.FragPos);
	float attenuation = 1.0 / (p_light.constant + p_light.linear * distance + p_light.quadratic * (distance * distance));

	BlinnPhongData data =
	{
		lightDir,
		g_viewDir,
		p_light.color,
		g_diffColor,
		u_specularColor,
		g_normal,
		u_shininess
	};

	return computeBlinnPhong(data) * attenuation;
}

vec3 calculateSpotLight(SpotLight p_light)
{
	vec3 lightDir = normalize(p_light.position - fs_in.FragPos);
	vec3 spotDir = normalize(-p_light.direction);

	float spotAngle = dot(lightDir, spotDir);

	if (spotAngle < p_light.outerCutoff)
		return vec3(0);

	float epsilon = p_light.cutoff - p_light.outerCutoff;
	float intensity = clamp((spotAngle - p_light.outerCutoff) / epsilon, 0.0, 1.0);

	float distance = length(p_light.position - fs_in.FragPos);
	float attenuation = 1.0 / (p_light.constant + p_light.linear * distance + p_light.quadratic * (distance * distance));

	BlinnPhongData data =
	{
		lightDir,
		g_viewDir,
		p_light.color,
		g_diffColor,
		u_specularColor,
		g_normal,
		u_shininess
	};

	return computeBlinnPhong(data) * attenuation * intensity;
}