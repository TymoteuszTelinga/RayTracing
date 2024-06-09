#include "AssetMenager.h"

#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>

#include "AssetImporter.h"

static std::map<std::filesystem::path, AssetType> s_AssetExtensionMap = {
    {".obj", AssetType::Mesh},
    {".mat", AssetType::Material}
};

static AssetType GetAssetTypeFromExtension(const std::filesystem::path& extension)
{
    if (s_AssetExtensionMap.find(extension) == s_AssetExtensionMap.end())
    {
        std::cerr << "Extension not suported\n";
        return AssetType::None;
    }

    return s_AssetExtensionMap.at(extension);
}

AssetMenager& AssetMenager::Get()
{
    static AssetMenager s_Instance;
    return s_Instance;
}

bool AssetMenager::IsAssetValid(UUID handle) const
{
    return handle != 0 && m_AssetRegistry.find(handle) != m_AssetRegistry.end();
}

bool AssetMenager::IsAssetLoaded(UUID handle) const
{
    return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
}

AssetType AssetMenager::GetAssetType(UUID handle) const
{
    return GetMetadata(handle).Type;
}

void AssetMenager::ImportAsset(const std::filesystem::path& filepath)
{
    UUID handle; //generat handle for new asset
    AssetMetadata metadata;
    metadata.FilePath = filepath;
    metadata.Type = GetAssetTypeFromExtension(filepath.extension());

    //load asset;
    m_AssetRegistry[handle] = metadata;
}

Ref<Asset> AssetMenager::GetAssetInt(UUID handle)
{
    if (!IsAssetValid(handle))
        return nullptr;

    Ref<Asset> asset;
    if (IsAssetLoaded(handle))
    {
        asset = m_LoadedAssets.at(handle);
    }
    else
    {
        const AssetMetadata metadata = GetMetadata(handle);
        asset = AssetImporter::LoadAsset(handle, metadata);
        if (!asset)
        {
            //load failed
            std::cerr << "asset load failed/n";
        }
        m_LoadedAssets[handle] = asset;
    }

    return asset;
}

AssetMetadata AssetMenager::GetMetadata(UUID handel) const
{
    static AssetMetadata s_EmptyMetadata;
    auto it = m_AssetRegistry.find(handel);
    if (it == m_AssetRegistry.end())
    {
        return s_EmptyMetadata;
    }

    return it->second;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const std::string_view& v)
{
    out << std::string(v.data(), v.size());
    return out;
}

void AssetMenager::SerializeAssetRegistry()
{
    YAML::Emitter out;

    out << YAML::BeginMap;
    out << YAML::Key << "AssetRegistry" << YAML::Value;

    out << YAML::BeginSeq;
    for (auto const& [handle, metadata] : m_AssetRegistry)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "UUID" << YAML::Value << handle;
        std::string filepathstr = metadata.FilePath.generic_string();
        out << YAML::Key << "FilePath" << YAML::Value << filepathstr;
        out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(metadata.Type);
        out << YAML::EndMap;
    }
    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::filesystem::path outPath = BasePath;
    outPath /= "Registy.reg";
    std::ofstream file(outPath);
    file << out.c_str();
}

bool AssetMenager::DeserializeAssetRegistry()
{
    std::cout << "start Init AssetMenager\n";
    std::filesystem::path outPath = BasePath;
    if (!std::filesystem::exists(outPath))
    {
        std::filesystem::create_directory(outPath);
        return false;
    }
    outPath /= "Registy.reg";
    YAML::Node data;
    try
    {
        data = YAML::LoadFile(outPath.generic_string());
    }
    catch (YAML::ParserException e)
    {
        return false;
    }

    auto registy = data["AssetRegistry"];
    if (!registy)
    {
        return false;
    }

    for (const auto& entry : registy)
    {
        UUID handle = entry["UUID"].as<uint64_t>();
        AssetMetadata& metadata = m_AssetRegistry[handle];
        metadata.FilePath = entry["FilePath"].as<std::string>();
        metadata.Type = AssetTypeFromString(entry["Type"].as<std::string>());
    }

    return true;
}

