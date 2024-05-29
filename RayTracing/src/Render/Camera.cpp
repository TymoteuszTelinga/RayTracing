#include "Camera.h"
#include "Core/Input.h"

#include <GLFW/glfw3.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <yaml-cpp/yaml.h>
#include <fstream>
namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};
}

static YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

Camera::Camera(float VOF, float nearClip, float farClip)
	:m_VerticalFOV(VOF), m_NearClip(nearClip), m_FarClip(farClip)
{
	YAML::Node data = YAML::LoadFile("camera.yaml");
	//m_Location = data["Position"].as<glm::vec3>();
	//m_Forward = data["Forward"].as<glm::vec3>();

	RecalculateView();
	RecalculateProjection();
}

Camera::~Camera()
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Position" << YAML::Value << m_Location;
	out << YAML::Key << "Forward" << YAML::Value << m_Forward;
	out << YAML::EndMap;

	std::ofstream fout("camera.yaml");
	fout << out.c_str();
}

bool Camera::OnUpdate(float DeltaTime)
{
	glm::vec2 MousePos = Input::GetMousePosition();
	glm::vec2 delta = (MousePos - m_LastMousePosition) * 0.02f;
	m_LastMousePosition = MousePos;

	if (!Input::IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
	{
		Input::SetCursorMode(GLFW_CURSOR_NORMAL);
		return false;
	}

	Input::SetCursorMode(GLFW_CURSOR_DISABLED);

	constexpr glm::vec3 UpVector(0.f, 1.f, 0.f);
	glm::vec3 RightVector = glm::cross(m_Forward, UpVector);
	RightVector = glm::normalize(RightVector);

	float Speed = 5.f;

	bool bMoved = false;
	//Movement
	if (Input::IsKeyDown(GLFW_KEY_W))
	{
		m_Location += m_Forward * Speed * DeltaTime;
		bMoved = true;
	}
	else if (Input::IsKeyDown(GLFW_KEY_S))
	{
		m_Location -= m_Forward * Speed * DeltaTime;
		bMoved = true;
	}

	if (Input::IsKeyDown(GLFW_KEY_D))
	{
		m_Location += RightVector * Speed * DeltaTime;
		bMoved = true;
	}
	else if (Input::IsKeyDown(GLFW_KEY_A))
	{
		m_Location -= RightVector * Speed * DeltaTime;
		bMoved = true;
	}

	if (Input::IsKeyDown(GLFW_KEY_E))
	{
		m_Location += UpVector * Speed * DeltaTime;
		bMoved = true;
	}
	else if (Input::IsKeyDown(GLFW_KEY_Q))
	{
		m_Location -= UpVector * Speed * DeltaTime;
		bMoved = true;
	}

	//rotation
	if (delta.x != 0.f || delta.y != 0.f)
	{
		float PitchDelta = -delta.y * 0.3;
		float YawDelta = -delta.x * 0.3;

		glm::quat q = glm::normalize(glm::cross(glm::angleAxis(PitchDelta, RightVector),
			glm::angleAxis(YawDelta, UpVector)));
		m_Forward = glm::rotate(q, m_Forward);
		bMoved = true;
	}

	if (bMoved)
	{
		RecalculateView();
	}

	return bMoved;
}

void Camera::OnResize(uint32_t width, uint32_t height)
{
	if (width == m_ViewportWidth && height == m_ViewportHeight)
	{
		return;
	}

	m_ViewportWidth = width;
	m_ViewportHeight = height;

	RecalculateProjection();
}

void Camera::SetFOV(float fov)
{
	m_VerticalFOV = fov;
	RecalculateProjection();
}

const glm::mat4& Camera::GetInversViewProjection()
{
	glm::mat4 VP = m_Projection * m_View;
	return glm::inverse(VP);
}

glm::vec3 Camera::GetRotation() const
{
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(m_View, scale, rotation, translation, skew, perspective);
	rotation = glm::conjugate(rotation);
	return glm::degrees(glm::eulerAngles(rotation));
}

void Camera::RecalculateProjection()
{
	m_Projection = glm::perspectiveFov(glm::radians(m_VerticalFOV), (float)m_ViewportWidth, (float)m_ViewportHeight, m_NearClip, m_FarClip);
	m_InverseProjection = glm::inverse(m_Projection);
}

void Camera::RecalculateView()
{
	glm::vec3 RightVector = glm::cross(m_Forward, glm::vec3(0.0, 1.0, 0.0));
	RightVector = glm::normalize(RightVector);

	glm::vec3 UpWector = glm::cross(RightVector, m_Forward);
	UpWector = glm::normalize(UpWector);

	m_View = glm::lookAt(m_Location, m_Location + m_Forward, UpWector);
	m_InversView = glm::inverse(m_View);
}
