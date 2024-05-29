#include "AssetImporter.h"

#include "Render/Mesh.h"
#include "MaterialSerializer.h"
#include <iostream>

Ref<Asset> AssetImporter::LoadAsset(UUID handle, const AssetMetadata& metadata)
{
	switch (metadata.Type)
	{
	case AssetType::Mesh:
		{
			Ref<Mesh> mesh = CreateRef<Mesh>(metadata.FilePath.generic_string());
			mesh->Handle = handle;
			return mesh;
		}
	case AssetType::Material:
		{
			Ref<Material> mat = CreateRef<Material>();
			MaterialSerializer serializer(mat);
			serializer.Deserialize(metadata.FilePath.generic_string());
			mat->Handle = handle;
			return mat;
		}
	}

	std::cerr << "No importer aveable for type";
	return nullptr;
}
