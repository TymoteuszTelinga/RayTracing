
#include "SceneSerializer.h"

#include "Object.h"
#include "Animations/Track.h"
#include <fstream>
#include <yaml-cpp/yaml.h>

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

	template<>
	struct convert<UUID>
	{
		static Node encode(const UUID& uuid)
		{
			Node node;
			node.push_back((uint64_t)uuid);
			return node;
		}

		static bool decode(const Node& node, UUID& uuid)
		{
			uuid = node.as<uint64_t>();
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

SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
	:m_Scene(scene)
{
}

static void SerializeObject(YAML::Emitter& out, const Object& object)
{
	out << YAML::BeginMap;

	out << YAML::Key << "UUID" << YAML::Value << object.GetID();
	out << YAML::Key << "Name" << YAML::Value << object.GetName();
	out << YAML::Key << "Position" << YAML::Value << object.GetPosition();
	out << YAML::Key << "Rotation" << YAML::Value << object.GetRotation();
	out << YAML::Key << "Scale" << YAML::Value << object.GetScale();
	out << YAML::Key << "Mesh" << YAML::Value << object.GetMeshID();
	out << YAML::Key << "Material" << YAML::Value << object.GetMaterialID();

	out << YAML::EndMap;
}

static void SerializeAnimation(YAML::Emitter& out, const UUID& objectUUID, Track& track)
{
	out << YAML::BeginMap;

	out << YAML::Key << "Object UUID" << YAML::Value << objectUUID;
	out << YAML::Key << "KeyFrames" << YAML::Value << YAML::BeginSeq;
	for (const KeyFrame& keyframe : track.GetTrackData())
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Frame" << YAML::Value << keyframe.frame;
		out << YAML::Key << "Value" << YAML::Value << keyframe.value;
		out << YAML::EndMap;
	}
	out << YAML::EndSeq;

	out << YAML::EndMap;
}

void SceneSerializer::Serialize(const std::string& filepath)
{
	YAML::Emitter out;

	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << "Untilted";
	out << YAML::Key << "Duration" << YAML::Value << m_Scene->m_AnimationDuration;
	out << YAML::Key << "Objects" << YAML::Value << YAML::BeginSeq;
	//serialize objects;
	for (auto const& [uuid, obj] : m_Scene->m_Objects)
	{
		SerializeObject(out, obj);
	}
	out << YAML::EndSeq;

	out << YAML::Key << "Animations" << YAML::Value << YAML::BeginSeq;
	//serialize animations;
	for (auto& [objectUUID, track] : m_Scene->m_Animations)
	{
		if (track.IsValid())
		{
			SerializeAnimation(out,objectUUID, track);
		}
	}
	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(filepath);
	fout << out.c_str();
}

bool SceneSerializer::Deserialize(const std::string& filepath)
{
	YAML::Node data = YAML::LoadFile(filepath);
	if (!data["Scene"])
	{
		return false;
	}

	std::string sceneName = data["Scene"].as<std::string>();

	if (data["Duration"])
	{
		m_Scene->m_AnimationDuration = data["Duration"].as<uint32_t>();
	}

	auto objects = data["Objects"];
	if (objects)
	{
		for (auto objectData : objects)
		{
			uint64_t uuid = objectData["UUID"].as<uint64_t>();
			std::string name = objectData["Name"].as<std::string>();
			Object& o = m_Scene->CreateObjectWithUUID(uuid, name);

			glm::vec3 position = objectData["Position"].as<glm::vec3>();
			glm::vec3 rotation = objectData["Rotation"].as<glm::vec3>();
			glm::vec3 scale = objectData["Scale"].as<glm::vec3>();
			o.SetTransform(position, rotation, scale);

			uint64_t material = objectData["Material"].as<uint64_t>();
			int index = m_Scene->GetMaterialIndex(material);
			o.SetMaterial(material);
			o.SetMaterialIndex(index);

			uint64_t mesh = objectData["Mesh"].as<uint64_t>();
			int MeshIndex = m_Scene->GetMeshIndex(mesh);
			o.SetMesh(mesh);
			o.SetMeshIndex(MeshIndex);
		}
	}

	auto animations = data["Animations"];
	if (animations)
	{
		for (auto animationData : animations)
		{
			uint64_t objectUUID = animationData["Object UUID"].as<uint64_t>();
			for (auto trackData : animationData["KeyFrames"])
			{
				uint32_t frame = trackData["Frame"].as<uint32_t>();
				glm::vec3 value = trackData["Value"].as<glm::vec3>();

				m_Scene->InsertKeyframe(objectUUID, frame, value);
			}
		}
	}

	m_Scene->Upload();

	return true;
}
