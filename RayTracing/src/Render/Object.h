#pragma once

#include <glm/glm.hpp>
#include <string>
#include "ShaderTypes.h"
#include "Core/UUID.h"

class Object
{
public:
	Object() {};
	Object(uint64_t uuid, ObjectInstance& Instance, const std::string& name);
	~Object();

	void SetPosition(glm::vec3 position);
	void SetInstance(ObjectInstance& Instance);
	glm::vec3& GetPosition() { return m_Position; };

	std::string GetName() const { return m_Name; };

	glm::mat4 GetTransform() const { return m_InstanceData->LocalToWorld; };

	//temorary solution
	glm::vec3 GetPosition() const { return m_Position; };
	glm::vec3 GetRotation() const { return m_Rotation; };
	glm::vec3 GetScale() const { return m_Scale; };

	UUID GetID() const { return m_UUID; };
	UUID GetMeshID() const { return m_MeshID; };
	UUID GetMaterialID() const { return m_MaterialID; };

	void SetTransform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	void SetTransform(glm::mat4 transform);

	void SetMaterial(UUID uuid);
	void SetMaterialIndex(int ID);

	void SetMesh(UUID uuid);
	void SetMeshIndex(int ID);


private:

	void RecalculateTransform();
	void DecomposeTransform(const glm::mat4 &transform);

private:
	UUID m_UUID;
	ObjectInstance* m_InstanceData = nullptr;

	std::string m_Name;

	UUID m_MeshID = 0;
	UUID m_MaterialID = 0;

	glm::vec3 m_Position;
	glm::vec3 m_Rotation;
	glm::vec3 m_Scale;

	friend class SceneHierarchyPanel;
};