#include "MaterialPanel.h"

#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include "Asset/AssetMenager.h"
#include "Asset/MaterialSerializer.h"

MaterialPanel::MaterialPanel(const Ref<Material>& mat)
    :m_Mat(mat)
{
}

void MaterialPanel::SetContext(const Ref<Material>& mat)
{
    m_Mat = mat;
}

bool MaterialPanel::OnImGuiRender()
{
    bool change = false;
    ImGui::Begin("Material Editor");
    SelectNew();
    if (m_Mat.get() == nullptr)
    {
        ImGui::End();
        return false;
    }

    MaterialInfo& details = m_Mat->m_Details;
    change |= ImGui::ColorEdit3("Albedo", glm::value_ptr(details.Albedo));
    change |= ImGui::DragFloat("Roughness", &details.Roughness, 0.01f, 0.0f, 1.0f);
    change |= ImGui::ColorEdit3("EmissiveColor", glm::value_ptr(details.EmissiveColor));
    change |= ImGui::DragFloat("EmissivePower", &details.EmissivePower, 0.01f, 0.0f, FLT_MAX);
    change |= ImGui::ColorEdit3("SpecularColor", glm::value_ptr(details.SpecularColor));
    change |= ImGui::DragFloat("SpecularChance", &details.SpecularChance, 0.01f, 0.0f, 1.0f);
	change |= ImGui::DragFloat("Metallic", &details.Metallic, 0.01f, 0.0f, 1.0f);
    ImGui::End();

    if (change)
    {
        AssetMetadata metadata = AssetMenager::Get().GetMetadata(m_Mat->Handle);
		MaterialSerializer Serializer(m_Mat);
        Serializer.Serialize(metadata.FilePath.generic_string());
    }
    return change;
}

void MaterialPanel::SelectNew()
{
	std::string DispalyName = "None";
	uint64_t SelectedID = 0;
	if (m_Mat.get())
	{
		auto metadata = AssetMenager::Get().GetMetadata(m_Mat->Handle);
		DispalyName = metadata.FilePath.filename().replace_extension().generic_string();
		SelectedID = m_Mat->Handle;
	}

	if (ImGui::BeginCombo("Material", DispalyName.c_str(), ImGuiComboFlags_NoArrowButton))
	{
		//GetAssetMenager material Info
		for (const auto& [handle, metadata] : AssetMenager::Get().GetAssetRegistry())
		{
			if (metadata.Type == AssetType::Material)
			{
				std::string name = metadata.FilePath.filename().replace_extension().generic_string();
				const bool bSelected = (SelectedID == handle);
				if (ImGui::Selectable(name.c_str(), bSelected))
				{
					m_Mat = AssetMenager::Get().GetAsset<Material>(handle);
				}
				if (bSelected)
					ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}
