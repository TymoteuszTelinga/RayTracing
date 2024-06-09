#pragma once

#include <map>
#include "Asset.h"
#include "Core/Base.h"

using AssetRegistry = std::map<UUID, AssetMetadata>;

class AssetMenager
{
public:
	AssetMenager() {};
	~AssetMenager() {};

	static AssetMenager& Get();

	template<typename T>
	Ref<T> GetAsset(UUID handle);
	bool IsAssetValid(UUID handle) const;
	bool IsAssetLoaded(UUID handle) const;
	AssetType GetAssetType(UUID handle) const;
	AssetMetadata GetMetadata(UUID handel) const;

	void ImportAsset(const std::filesystem::path& filepath);
	template<typename T>
	Ref<T> CreateAsset(const std::filesystem::path& filepath);

	const AssetRegistry& GetAssetRegistry() const { return m_AssetRegistry; };

	void SerializeAssetRegistry();
	bool DeserializeAssetRegistry();
	std::filesystem::path GetBasePath() const { return BasePath; };

private:
	Ref<Asset> GetAssetInt(UUID handle);

private:
	std::filesystem::path BasePath = "src/Assecs";
	AssetRegistry m_AssetRegistry;
	std::map<UUID, Ref<Asset>> m_LoadedAssets;
};

template<typename T>
inline Ref<T> AssetMenager::GetAsset(UUID handle)
{
	Ref<Asset> asset = GetAssetInt(handle);
	return std::static_pointer_cast<T>(asset);
}

template<typename T>
inline Ref<T> AssetMenager::CreateAsset(const std::filesystem::path& filepath)
{
	static_assert(std::is_base_of<Asset, T>::value, "Canot create asset of type");
	Ref<Asset> asset = std::static_pointer_cast<Asset>(CreateRef<T>());
	UUID handle = asset->Handle;
	AssetMetadata metadata;
	metadata.FilePath = filepath;
	metadata.Type = asset->GetType();

	m_AssetRegistry[handle] = metadata;
	m_LoadedAssets[handle] = asset;

	return std::static_pointer_cast<T>(asset);
}
