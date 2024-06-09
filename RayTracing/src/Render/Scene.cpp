#include "Scene.h"
#include "Object.h"
#include "Buffer.h"
#include "Asset/AssetMenager.h"
#include "Material.h"

Scene::Scene()
{
	m_TriangleBuffer = CreateScope<Buffer<Triangle>>();
	m_TriangleBuffer->Bind(1);

	m_VertexBuffer = CreateScope<Buffer<Vertex>>();
	m_VertexBuffer->Bind(2);

	m_MeshBuffer = CreateScope<Buffer<MeshInstance>>();
	m_MeshBuffer->Bind(3);

	m_Instances = CreateScope<Buffer<ObjectInstance>>();
	m_Instances->Bind(4);

	m_MaterialBuffer = CreateScope<Buffer<MaterialInfo>>();
	m_MaterialBuffer->Bind(5);
}

Scene::~Scene()
{
}

//create new empty object
Object& Scene::CreateObject(const std::string& name)
{
	return CreateObjectWithUUID(UUID(), name);
}

Object& Scene::CreateObjectWithUUID(UUID uuid, const std::string& name)
{
	ObjectInstance empty;
	empty.WorldToLocal = glm::mat4(1.0f);
	empty.LocalToWorld = glm::mat4(1.0f);

	m_Instances->AddElement(empty);
	int index = m_Instances->GetSize() - 1;

	auto ret = m_Objects.emplace(uuid, Object(uuid, m_Instances->GetElement(index), name));
	return ret.first->second;
}

void Scene::RemoveObject(const UUID& object)
{

	m_Objects.erase(object);
	m_Animations.erase(object);
}

void Scene::AddMesh(const Ref<Mesh>& mesh)
{
	//chek mesh is in a scene
	if (m_Meshes.find(mesh->GetID()) != m_Meshes.end())
	{
		return;
	}

	m_Meshes[mesh->GetID()] = m_MeshBuffer->GetSize();

	MeshInstance MeshInstance;
	MeshInstance.Min = mesh->GetBBoxMin();
	MeshInstance.Max = mesh->GetBBoxMax();
	MeshInstance.StartFace = m_TriangleBuffer->GetSize();
	MeshInstance.Size = mesh->GetFaces().size();

	m_MeshBuffer->AddElement(MeshInstance);

	//add mesh data to buffers
	glm::ivec3 MeshOffset(m_VertexBuffer->GetSize());
	std::vector<Triangle> MeshTriangles(mesh->GetFaces());
	for (Triangle& T : MeshTriangles)
	{
		T.Vertexis += MeshOffset;
	}
	m_TriangleBuffer->AddElements(MeshTriangles);
	m_VertexBuffer->AddElements(mesh->GetVertecis());


	//Update all buffers
	m_TriangleBuffer->Bind(1);
	m_TriangleBuffer->Update();

	m_VertexBuffer->Bind(2);
	m_VertexBuffer->Update();

	m_MeshBuffer->Bind(3);
	m_MeshBuffer->Update();
}

int Scene::GetMeshIndex(UUID handle)
{
	auto it = m_Meshes.find(handle);
	if (it != m_Meshes.end())
	{
		return it->second;
	}

	if (!AssetMenager::Get().IsAssetValid(handle))
	{
		return 0;
	}

	Ref<Mesh> mesh = AssetMenager::Get().GetAsset<Mesh>(handle);
	AddMesh(mesh);
	return m_Meshes.at(handle);
}

int Scene::GetMaterialIndex(UUID handle)
{
	auto it = m_Materials.find(handle);
	if (it != m_Materials.end())
	{
		return it->second;
	}

	if (!AssetMenager::Get().IsAssetValid(handle))
	{
		return 0;
	}
	
	Ref<Material> mat = AssetMenager::Get().GetAsset<Material>(handle);
	int index = m_MaterialBuffer->GetSize();
	m_MaterialBuffer->AddElement(mat->GetDetails());
	m_MaterialBuffer->Bind(5);
	m_MaterialBuffer->Update();
	m_Materials[handle] = index;
	return index;
}

int Scene::GetObjectsCount() const
{
	return m_Instances->GetSize();
}

void Scene::InsertKeyframe(const UUID& object, const uint32_t frame, const glm::vec3 value)
{
	m_Animations[object].AddKeyFrame({ frame,value });
}

void Scene::Update(const uint32_t frame)
{
	m_ActiveFrame = frame;
	for(const auto &[objectID, track]: m_Animations)
	{
		if (!track.IsValid())
		{
			continue;
		}
		glm::vec3 pos = track.GetValue(frame);
		m_Objects[objectID].SetPosition(pos);
	}

	m_Instances->Bind(4);
	m_Instances->Update();
}

void Scene::Upload()
{
	m_TriangleBuffer->Bind(1);
	m_TriangleBuffer->Update();

	m_VertexBuffer->Bind(2);
	m_VertexBuffer->Update();

	m_MeshBuffer->Bind(3);
	m_MeshBuffer->Update();

	m_Instances->Bind(4);
	m_Instances->Update();
}

void Scene::UpdateMaterial(const Ref<Material>& mat)
{
	auto res = m_Materials.count(mat->Handle);
	if (res == 0)
	{
		return;
	}

	uint16_t index = m_Materials.at(mat->Handle);
	m_MaterialBuffer->GetElement(index) = mat->GetDetails();
	m_MaterialBuffer->Bind(5);
	m_MaterialBuffer->Update();
}
