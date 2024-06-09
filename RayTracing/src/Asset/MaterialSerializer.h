#pragma once

#include "Core/Base.h"
#include "Render/Material.h"

class MaterialSerializer
{
public:
	MaterialSerializer(const Ref<Material>& mat);

	void Serialize(const std::string& filepath);
	bool Deserialize(const std::string& filepath);

private:
	Ref<Material> m_Mat;
};