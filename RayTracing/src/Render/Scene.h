#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include "Core/Base.h"
#include "Core/UUID.h"
#include "ShaderTypes.h"
#include "Object.h"
#include "Mesh.h"
#include "Buffer.h"
#include "Material.h"

#include "Animations/Track.h"

class Scene
{
public:
	Scene();
	~Scene();

	Object& CreateObject(const std::string& name = std::string());
	Object& CreateObjectWithUUID(UUID uuid, const std::string& name);
	void RemoveObject(const UUID& object);

	int GetMeshIndex(UUID handle);
	int GetMaterialIndex(UUID handle);

	int GetObjectsCount() const;
	void InsertKeyframe(const UUID& object, const uint32_t frame, const glm::vec3 value);

	void Update(const uint32_t frame);
	void Upload();
	void UpdateMaterial(const Ref<Material>& mat);

	uint32_t GetSceneDuration() const { return m_AnimationDuration; };

private:
	void AddMesh(const Ref<Mesh>& mesh);
private:
	//RenderData
	Scope<Buffer<Triangle>> m_TriangleBuffer;
	Scope<Buffer<Vertex>> m_VertexBuffer;
	Scope<Buffer<MeshInstance>> m_MeshBuffer;
	Scope<Buffer<ObjectInstance>> m_Instances;
	Scope<Buffer<MaterialInfo>> m_MaterialBuffer;

	//local Mesh Registry
	std::unordered_map<UUID, uint16_t> m_Meshes;
	std::unordered_map<UUID, uint16_t> m_Materials;

	//Scene Data
	std::map<UUID, Object> m_Objects;
	std::map<UUID, Track> m_Animations;

	uint32_t m_ActiveFrame = 0;
	uint32_t m_AnimationDuration = 100;

	friend class SceneHierarchyPanel;
	friend class SceneSerializer;
	friend class TimeLinePanel;
};