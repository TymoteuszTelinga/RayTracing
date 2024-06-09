#include "MaterialSerializer.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

//taken form SceneSerializer
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

MaterialSerializer::MaterialSerializer(const Ref<Material>& mat)
	:m_Mat(mat)
{
}

void MaterialSerializer::Serialize(const std::string& filepath)
{
	YAML::Emitter out;
	MaterialInfo matInfo = m_Mat->GetDetails();

	out << YAML::BeginMap;
	out << YAML::Key << "Material" << YAML::Value << YAML::BeginMap;
	out << YAML::Key << "Albedo" << YAML::Value << matInfo.Albedo;
	out << YAML::Key << "Roughness" << YAML::Value << matInfo.Roughness;
	out << YAML::Key << "Emission Color" << YAML::Value << matInfo.EmissiveColor;
	out << YAML::Key << "Emission Power" << YAML::Value << matInfo.EmissivePower;
	out << YAML::Key << "Specular Color" << YAML::Value << matInfo.SpecularColor;
	out << YAML::Key << "Specular Chance" << YAML::Value << matInfo.SpecularChance;
	out << YAML::EndMap;
	out << YAML::EndMap;

	std::ofstream file(filepath);
	file << out.c_str();
}

bool MaterialSerializer::Deserialize(const std::string& filepath)
{
	YAML::Node data = YAML::LoadFile(filepath);
	if (!data["Material"])
	{
		return false;
	}

	auto mat = data["Material"];
	if (!mat)
	{
		return false;
	}

	MaterialInfo info;
	info.Albedo = mat["Albedo"].as<glm::vec3>();
	info.Roughness = mat["Roughness"].as<float>();
	info.EmissiveColor = mat["Emission Color"].as<glm::vec3>();
	info.EmissivePower = mat["Emission Power"].as<float>();
	info.SpecularColor = mat["Specular Color"].as<glm::vec3>();
	info.SpecularChance = mat["Specular Chance"].as<float>();

	m_Mat->SetMaterialDetails(info);
	return true;
}
