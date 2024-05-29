#pragma once

#include "Render/Scene.h"
#include "Core/Base.h"
#include "Core/UUID.h"

class Object;

class SceneHierarchyPanel
{
public:

	SceneHierarchyPanel(const Ref<Scene>& scene);
	void SetContext(const Ref<Scene>& scene);

	void OnImGuiRender();
	Object* GetSelected();

private:

	bool DisplayMeshes(Object& object) const;
	bool DisplayMaterials(Object& object) const;
	void DrawProperties(const UUID& selected);

private:
	Ref<Scene> m_Context;
	UUID m_Selected;
};