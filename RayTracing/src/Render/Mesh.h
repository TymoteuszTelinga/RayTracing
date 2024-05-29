#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "ShaderTypes.h"
#include "Core/UUID.h"
#include "Asset/Asset.h"
#include "Core/Base.h"

class Mesh : public Asset
{
public:
	Mesh(const std::string& filePath);
	~Mesh();

	const std::vector<Triangle>& GetFaces() const { return m_Faces; };
	const std::vector<Vertex>& GetVertecis() const { return m_Vertesis; };

	virtual AssetType GetType() const override { return AssetType::Mesh; };
	const UUID GetID() const { return Handle; };
	glm::vec3 GetBBoxMin() const { return m_MinBox; };
	glm::vec3 GetBBoxMax() const { return m_MaxBox; };

private:
	void UpdateBBox(const glm::vec3& position);

private:
	std::vector<Vertex> m_Vertesis;
	std::vector<Triangle> m_Faces;
	glm::vec3 m_MinBox;
	glm::vec3 m_MaxBox;

};

