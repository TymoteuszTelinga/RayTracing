#pragma once

#include "Asset/Asset.h"
#include "ShaderTypes.h"

class Material : public Asset
{	
public:
	virtual AssetType GetType() const { return AssetType::Material; };
	MaterialInfo GetDetails() const { return m_Details; };
	void SetMaterialDetails(const MaterialInfo& details) { m_Details = details; };

private:
	MaterialInfo m_Details;

	friend class MaterialPanel;
};