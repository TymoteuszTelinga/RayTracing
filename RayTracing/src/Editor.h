#pragma once
#include "Core/Layer.h"
#include "Core/Base.h"
#include "Render/Scene.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/TimeLinePanel.h"
#include "Panels/MaterialPanel.h"

class Image;
class Program;
class Camera;

class Editor : public Layer
{
public:
	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(float DeltaTime) override;
	virtual void OnUIRender() override;

private:

	void NewScene();
	void MaterialModal(bool& isOpen);

private:

	int32_t m_ImageWidth = 720;
	int32_t m_ImageHeight = 720;
	int32_t m_Pasess = 1;
	float m_LastTimeRender = 0.0f;
	float m_CameraFOV = 45.0f;
	uint32_t m_Frame = 0;
	bool bViewPortActive = false;

	bool m_Open = true;
	bool m_MaterialModalOpen = false;
	bool bAcumutale = false;

	int m_GizmoType = 0;

	Ref<Camera> m_Camera;
	Ref<Scene> m_ActiveScene;

	Scope<SceneHierarchyPanel> m_HierarhchyPanel;
	Scope<TimeLinePanel> m_TimeLinePanel;
	Scope<MaterialPanel> m_MaterialPanel;
};

