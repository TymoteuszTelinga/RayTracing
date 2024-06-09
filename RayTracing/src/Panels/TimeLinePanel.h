#pragma once

#include <cstdint>
#include <vector>
#include "Core/Base.h"
#include "Render/Scene.h"
#include "Render/Animations/Track.h"

class TimeLinePanel
{
public:
	TimeLinePanel(const Ref<Scene>& scene);
	void SetContext(const Ref<Scene>& scene);

	void OnUpdate(float deltaTime);
	void OnImGuiRender();

	uint32_t GetCurentFrame() const { return m_CurrentFrame; };
	void SetFrame(uint32_t Frame) { m_CurrentFrame = Frame; };
private:

	void DrawBackGround();
	void DrawTrack(Track& track);
	void DrawMarker();

private:
	Ref<Scene> m_Context;
	uint32_t m_CurrentFrame = 0;
	float m_TimeLastUpdate = 0.0f;

	enum class ScaneState
	{
		Edit = 0, Play = 1
	};
	ScaneState m_SceneState = ScaneState::Edit;
};
