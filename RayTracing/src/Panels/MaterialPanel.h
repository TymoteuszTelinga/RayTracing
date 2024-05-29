#pragma once

#include "Core/Base.h"
#include "Render/Material.h"

class MaterialPanel
{
public:
	MaterialPanel(const Ref<Material>& mat);

	void SetContext(const Ref<Material>& mat);
	bool OnImGuiRender();

	Ref<Material> GetMaterial() const { return m_Mat; };

private:
	void SelectNew();

private:
	Ref<Material> m_Mat;
};