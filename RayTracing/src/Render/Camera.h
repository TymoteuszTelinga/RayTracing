#pragma once
#include <glm/glm.hpp>

class Camera
{
public:
	Camera(float VOF = 45.f, float nearClip = 0.1f, float farClip = 100.f);
	~Camera();

	bool OnUpdate(float DeltaTime);
	void OnResize(uint32_t width, uint32_t height);
	void SetFOV(float fov);

	const glm::mat4& GetProjection() { return m_Projection; }
	const glm::mat4& GetView() { return m_View; }
	const glm::mat4& GetInverseProjection() { return m_InverseProjection; }
	const glm::mat4& GetInversView() { return m_InversView; }
	const glm::mat4& GetInversViewProjection();

	const glm::vec3& GetLocation() { return m_Location; }
	const glm::vec3& GetForward() { return m_Forward; }
	glm::vec3 GetRotation() const;

private:
	void RecalculateProjection();
	void RecalculateView();

private:
	glm::mat4 m_Projection { 1.0f };
	glm::mat4 m_View { 1.0f };
	glm::mat4 m_InverseProjection { 1.0f };
	glm::mat4 m_InversView { 1.0f };

	float m_VerticalFOV;
	float m_NearClip;
	float m_FarClip;

	glm::vec3 m_Location { 0.0f, 0.0f, 5.0f };
	glm::vec3 m_Forward { 0.0f, 0.0f, -1.0f };

	glm::vec2 m_LastMousePosition { 0.0f, 0.0f };

	uint32_t m_ViewportWidth = 10, m_ViewportHeight = 10;
};

