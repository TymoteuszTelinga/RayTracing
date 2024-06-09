
#include "SceneHierarchyPanel.h"
#include "imgui/imgui.h"

#include "Asset/AssetMenager.h"

SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
	:m_Context(scene), m_Selected(0)
{
}

void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
{
	m_Context = scene;
	m_Selected = 0;
}

void SceneHierarchyPanel::OnImGuiRender()
{
	ImGui::Begin("Scene");
	std::vector<UUID> ObjectToRemove;
	int ImGuiID = 0;
	for (auto& [uuid, object] : m_Context->m_Objects)
	{
		ImGuiID++;
		ImGui::PushID(ImGuiID);

		if (ImGui::Selectable(object.m_Name.c_str(), uuid == m_Selected))
			m_Selected = uuid;

		//pop up menu
		if (ImGui::BeginPopupContextItem())
		{
			m_Selected = uuid;
			if (ImGui::MenuItem("Insert KeyFrame ..."))
			{
				uint32_t frame = m_Context->m_ActiveFrame;
				m_Context->InsertKeyframe(uuid, frame, object.GetPosition());
			}
			if (ImGui::MenuItem("Delete"))
			{
				ObjectToRemove.emplace_back(uuid);
			}
			ImGui::EndPopup();
		}

		ImGui::PopID();
	}
	ImGui::Separator();

	ImGui::InvisibleButton("XX", ImGui::GetContentRegionAvail());
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Add object"))
		{
			m_Selected = m_Context->CreateObject("Object").GetID();
			m_Context->Upload();
		}
		ImGui::EndPopup();
	}
	

	ImGui::End();


	ImGui::Begin("Properties");
	if (m_Selected != 0)
	{
		DrawProperties(m_Selected);
	}

	ImGui::End();

	for (UUID& uuid : ObjectToRemove)
	{
		m_Context->RemoveObject(uuid);
		m_Selected = 0;
	}
}

Object* SceneHierarchyPanel::GetSelected()
{
	if (m_Selected == 0)
	{
		return nullptr;
	}

	return &m_Context->m_Objects[m_Selected];
}

bool SceneHierarchyPanel::DisplayMeshes(Object& object) const
{
	bool change = false;
	auto metadata = AssetMenager::Get().GetMetadata(object.GetMeshID());
	std::string DispalyName = "None";
	if (metadata.Type != AssetType::None)
	{
		DispalyName = metadata.FilePath.filename().replace_extension().generic_string();
	}

	if (ImGui::BeginCombo("Mesh", DispalyName.c_str(), ImGuiComboFlags_NoArrowButton))
	{
		//GetAssetMenager mesh Info
		for (const auto& [handle, metadata] : AssetMenager::Get().GetAssetRegistry())
		{
			if (metadata.Type == AssetType::Mesh)
			{
				std::string name = metadata.FilePath.filename().replace_extension().generic_string();
				const bool bSelected = (object.m_MeshID == handle);
				if (ImGui::Selectable(name.c_str(), bSelected))
				{
					object.m_MeshID = handle;
					object.m_InstanceData->MeshID = m_Context->GetMeshIndex(handle);
					change = true;
				}
				if (bSelected)
					ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	return change;
}

bool SceneHierarchyPanel::DisplayMaterials(Object& object) const
{
	bool change = false;
	auto metadata = AssetMenager::Get().GetMetadata(object.GetMaterialID());
	std::string DispalyName = "None";
	if (metadata.Type == AssetType::Material)
	{
		DispalyName = metadata.FilePath.filename().replace_extension().generic_string();
	}

	if (ImGui::BeginCombo("Material", DispalyName.c_str(), ImGuiComboFlags_NoArrowButton))
	{
		//GetAssetMenager material Info
		for (const auto& [handle, metadata] : AssetMenager::Get().GetAssetRegistry())
		{
			if (metadata.Type == AssetType::Material)
			{
				std::string name = metadata.FilePath.filename().replace_extension().generic_string();
				const bool bSelected = (object.m_MaterialID == handle);
				if (ImGui::Selectable(name.c_str(), bSelected))
				{
					object.m_MaterialID = handle;
					object.m_InstanceData->MaterialID = m_Context->GetMaterialIndex(handle);
					change = true;
				}
				if (bSelected)
					ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	return change;
}

void SceneHierarchyPanel::DrawProperties(const UUID& selected)
{
	bool change = false;

	Object& object = m_Context->m_Objects.at(selected);

	char buffer[256];
	memset(buffer, 0, sizeof(buffer));
	strcpy_s(buffer, sizeof(buffer), object.m_Name.c_str());
	if (ImGui::InputText("Name", buffer, sizeof(buffer)))
	{
		object.m_Name = std::string(buffer);
		change = true;
	}

	ImGui::Separator();
	change |= ImGui::DragFloat3("Position", &object.m_Position.x);
	glm::vec3 rotation = glm::degrees(object.m_Rotation);
	if (ImGui::DragFloat3("Rotation", &rotation.x))
	{
		object.m_Rotation = glm::radians(rotation);
		change |= true;
	}
	
	change |= ImGui::DragFloat3("Scale", &object.m_Scale.x);

	ImGui::Separator();
	change |= DisplayMeshes(object);
	change |= DisplayMaterials(object);

	if (change)
	{
		object.RecalculateTransform();
		m_Context->Upload();
	}
}
