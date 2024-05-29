
#include "Object.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

Object::Object(uint64_t uuid, ObjectInstance& Instance, const std::string& name)
    :m_UUID(uuid),  m_Name(name), m_InstanceData(&Instance), m_Scale(1.0f), m_Position(0.0f), m_Rotation(0.0f)
{
}

Object::~Object()
{
}

void Object::SetPosition(glm::vec3 position)
{
    m_Position = position;
    RecalculateTransform();
}

void Object::SetInstance(ObjectInstance& Instance)
{
	m_InstanceData = &Instance;
	RecalculateTransform();
}

void Object::SetTransform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    m_Position = position;
    m_Rotation = rotation;
    m_Scale = scale;
    RecalculateTransform();
}

void Object::SetTransform(glm::mat4 transform)
{
    m_InstanceData->LocalToWorld = transform;
    m_InstanceData->WorldToLocal = glm::inverse(transform);
    DecomposeTransform(transform);
}

void Object::SetMaterial(UUID uuid)
{
	m_MaterialID = uuid;
}

void Object::SetMaterialIndex(int ID)
{
    m_InstanceData->MaterialID = ID;
}

void Object::SetMesh(UUID uuid)
{
	m_MeshID = uuid;
}

void Object::SetMeshIndex(int ID)
{
	m_InstanceData->MeshID = ID;
}

void Object::RecalculateTransform()
{
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), m_Position);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_Scale);

    glm::quat quatRotation = glm::quat(m_Rotation);
    glm::mat4 rotation = glm::toMat4(quatRotation);

    glm::mat4 transform = translate * rotation * scale;
    m_InstanceData->LocalToWorld = transform;
    m_InstanceData->WorldToLocal = glm::inverse(scale) * glm::inverse(rotation) * glm::inverse(translate);
}

void Object::DecomposeTransform(const glm::mat4& transform)
{
	// From glm::decompose in matrix_decompose.inl

	using namespace glm;
	using T = float;

	mat4 LocalMatrix(transform);

	// Normalize the matrix.
	if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
		return;

	// First, isolate perspective.  This is the messiest.
	if (
		epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
		epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
		epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
	{
		// Clear the perspective partition
		LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
		LocalMatrix[3][3] = static_cast<T>(1);
	}

	// Next take care of translation (easy).
	m_Position = vec3(LocalMatrix[3]);
	LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

	vec3 Row[3], Pdum3;

	// Now get scale and shear.
	for (length_t i = 0; i < 3; ++i)
		for (length_t j = 0; j < 3; ++j)
			Row[i][j] = LocalMatrix[i][j];

	// Compute X scale factor and normalize first row.
	m_Scale.x = length(Row[0]);
	Row[0] = detail::scale(Row[0], static_cast<T>(1));
	m_Scale.y = length(Row[1]);
	Row[1] = detail::scale(Row[1], static_cast<T>(1));
	m_Scale.z = length(Row[2]);
	Row[2] = detail::scale(Row[2], static_cast<T>(1));

	// At this point, the matrix (in rows[]) is orthonormal.
	// Check for a coordinate system flip.  If the determinant
	// is -1, then negate the matrix and the scaling factors.
#if 0
	Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
	if (dot(Row[0], Pdum3) < 0)
	{
		for (length_t i = 0; i < 3; i++)
		{
			scale[i] *= static_cast<T>(-1);
			Row[i] *= static_cast<T>(-1);
		}
	}
#endif

	vec3 rotation;
	rotation.y = asin(-Row[0][2]);
	if (cos(rotation.y) != 0) {
		rotation.x = atan2(Row[1][2], Row[2][2]);
		rotation.z = atan2(Row[0][1], Row[0][0]);
	}
	else {
		rotation.x = atan2(-Row[2][0], Row[1][1]);
		rotation.z = 0;
	}

	vec3 deltaRoation = rotation - m_Rotation;
	m_Rotation += deltaRoation;
}
