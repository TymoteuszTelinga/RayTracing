#pragma once

#include "Asset.h"
#include "Core/Base.h"

class AssetImporter
{
public:
	static Ref<Asset> LoadAsset(UUID handle, const AssetMetadata& metadata);
};
