#pragma once

#include <filesystem>
#include "Core/UUID.h"

enum class AssetType : uint16_t
{
	None = 0,
	Material,
	Mesh
};

std::string_view AssetTypeToString(AssetType type);
AssetType AssetTypeFromString(const std::string& stringType);

class Asset
{
public:
	UUID Handle;
	virtual AssetType GetType() const = 0;
};

struct AssetMetadata
{
	AssetType Type = AssetType::None;
	std::filesystem::path FilePath;

	operator bool() const { return Type != AssetType::None; };//potencjalnie do usuniêcia
};

