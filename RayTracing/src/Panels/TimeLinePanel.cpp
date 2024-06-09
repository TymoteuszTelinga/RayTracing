
#include "TimeLinePanel.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"


TimeLinePanel::TimeLinePanel(const Ref<Scene>& scene)
	:m_Context(scene)
{
}

void TimeLinePanel::SetContext(const Ref<Scene>& scene)
{
	m_Context = scene;
}

void TimeLinePanel::OnUpdate(float deltaTime)
{
	if (m_SceneState == ScaneState::Edit)
	{
		return;
	}

	const float timePerFrame = 1.0f / 30.0f; //30fps
	m_TimeLastUpdate += deltaTime;

	if (m_TimeLastUpdate >= timePerFrame)
	{
		m_CurrentFrame++;
		m_TimeLastUpdate = 0;
	}

	if (m_CurrentFrame > m_Context->m_AnimationDuration)
	{
		m_CurrentFrame = 0;
	}

}

void TimeLinePanel::OnImGuiRender()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
	ImGui::Begin("Timeline");
	
	if (ImGui::Button("-")  && m_CurrentFrame > 0)
	{
		m_CurrentFrame--;
	}
	ImGui::SameLine();
	if (ImGui::Button("+") && m_CurrentFrame < m_Context->m_AnimationDuration)
	{
		m_CurrentFrame++;
	}
	ImGui::SameLine();
	ImGui::Text("Frame: %d", m_CurrentFrame);
	ImGui::SameLine();

	switch (m_SceneState)
	{
	case ScaneState::Edit:
		if (ImGui::Button("Play"))
		{
			m_SceneState = ScaneState::Play;
		}
		break;
	case ScaneState::Play:
		if (ImGui::Button("Stop"))
		{
			m_SceneState = ScaneState::Edit;
		}
		break;
	}

	ImGui::SameLine();
	ImGui::DragScalar("Duration", ImGuiDataType_U32, &m_Context->m_AnimationDuration, 0.5);

	DrawBackGround();
	DrawMarker();

	for (auto& [object, track] : m_Context->m_Animations)
	{
		if (track.IsValid())
		{
			DrawTrack(track);
		}
	}
	
	ImGui::End();
	ImGui::PopStyleVar();
}

void TimeLinePanel::DrawBackGround()
{
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	ImVec2 canvasPos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
	ImVec2 canvasSize = ImGui::GetContentRegionAvail();

	const float barWidth = canvasSize.x / m_Context->m_AnimationDuration;
	const int step = 50 / barWidth > 1 ? 50 / barWidth : 1;
	

	//draw frames bars
	for (int i = 0; i < m_Context->m_AnimationDuration; i += step)
	{
		ImVec2 RectStart = canvasPos + ImVec2(barWidth * i, 12);
		ImVec2 RectEnd = RectStart + ImVec2(0, canvasSize.y);
		drawList->AddLine(RectStart, RectEnd, IM_COL32(100, 100, 100, 255), 2);

		char tmp[512];
		ImFormatString(tmp, IM_ARRAYSIZE(tmp), "%d", i);
		ImVec2 textSize = ImGui::CalcTextSize(tmp);
		float xOffset = RectStart.x - textSize.x / 2;
		if (xOffset >= canvasPos.x)
		{
			drawList->AddText(ImVec2(xOffset, canvasPos.y), 0xFFBBBBBB, tmp);
		}
	}
	ImGui::InvisibleButton("XX",ImVec2(5,12));
}

void TimeLinePanel::DrawTrack(Track& track)
{
	float lineHeight = 18;

	ImDrawList* drawList = ImGui::GetWindowDrawList();
	ImVec2 canvasPos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
	ImVec2 canvasSize = ImGui::GetContentRegionAvail();
	ImVec2 cursorRowPos = ImGui::GetCursorPos();

	std::vector<KeyFrame> &trackFrames = track.GetTrackData();

	float barWidth = canvasSize.x / m_Context->m_AnimationDuration;
	{
		int startFrame = trackFrames.front().frame;
		int endFrame = trackFrames.back().frame;
		ImVec2 RectStart = canvasPos + ImVec2(barWidth * startFrame, 0);
		ImVec2 RectEnd = canvasPos + ImVec2(barWidth * endFrame, lineHeight);
		drawList->AddRectFilled(RectStart, RectEnd, IM_COL32(0, 0, 100, 105),5.f);
	}

	for(auto it = trackFrames.begin(); it != trackFrames.end();)
	{
		uint32_t frame = (*it).frame;
		ImGui::PushID(frame);
		ImGui::SetCursorPos(cursorRowPos + ImVec2(barWidth * frame, lineHeight/2) - ImVec2(3, 3));
		if (ImGui::Button("##", ImVec2(6, 6)))
		{
			it = trackFrames.erase(it);
		}
		else
		{
			it++;
		}
		ImGui::PopID();
	}

	ImGui::SetCursorPos(cursorRowPos);
	ImGui::InvisibleButton("XX", ImVec2(5, lineHeight));
}

void TimeLinePanel::DrawMarker()
{

	ImDrawList* drawList = ImGui::GetWindowDrawList();
	ImVec2 canvasPos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
	ImVec2 canvasSize = ImGui::GetContentRegionAvail();

	float barWidth = canvasSize.x / m_Context->m_AnimationDuration;
	//draw marker
	{
		ImVec2 RectStart = canvasPos + ImVec2(barWidth * m_CurrentFrame, 0);
		ImVec2 RectEnd = RectStart + ImVec2(0, canvasSize.y);
		drawList->AddLine(RectStart, RectEnd, IM_COL32(80, 0, 0, 255), 2);

		if (ImGui::GetIO().MouseDown[ImGuiMouseButton_Left] && ImGui::IsMouseHoveringRect(canvasPos, canvasPos + canvasSize))
		{
			float dragDistance = ImGui::GetIO().MousePos.x - canvasPos.x;
			m_CurrentFrame = ((dragDistance + barWidth / 2) / barWidth);
		}
	}
}
