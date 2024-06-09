#include "Asset.h"

std::string_view AssetTypeToString(AssetType type)
{
	switch (type)
	{
	case AssetType::None:		return "AssetType::None";
	case AssetType::Material:	return "AssetType::Material";
	case AssetType::Mesh:		return "AssetType::Mesh";
	}

	return "AssetType::Invalid";
}

AssetType AssetTypeFromString(const std::string& stringType)
{
	if (stringType == "AssetType::None")		return AssetType::None;
	if (stringType == "AssetType::Material")	return AssetType::Material;
	if (stringType == "AssetType::Mesh")		return AssetType::Mesh;

	return AssetType::None;
}
