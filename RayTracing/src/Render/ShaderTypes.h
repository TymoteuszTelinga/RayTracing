#pragma once

#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 Position;
	float padding;
};

struct Triangle
{
	glm::ivec3 Vertexis;
	int _padding;
};

struct MeshInstance
{
	glm::vec3 Min;
	int StartFace;
	glm::vec3 Max;
	int Size;
};

struct ObjectInstance
{
	glm::mat4 LocalToWorld;
	glm::mat4 WorldToLocal;
	int MeshID = 0;
	int MaterialID = 0;
	glm::ivec2 _Padding;
};

struct MaterialInfo
{
	glm::vec3 Albedo{1.0f};
	float Roughness = 1.0f;
	glm::vec3 EmissiveColor{1.0f};
	float EmissivePower = 0.0f;
	glm::vec3 SpecularColor{1.0f};
	float SpecularChance = 0.0f;
	//exp
	glm::vec3 padding;
	float Metallic = 0.0f;
};