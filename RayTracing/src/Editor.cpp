#include "Editor.h"
#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"

#include "Core/Timer.h"
#include "Core/Input.h"
#include "Core/FileDialog.h"
#include <GLFW/glfw3.h> // custom key codes

#include "Render/Image.h"
#include "Render/Program.h"
#include "Render/Camera.h"
#include "Render/Buffer.h"
#include "Render/Mesh.h"
#include "Render/Material.h"
#include "Render/SceneSerializer.h"
#include "Render/Renderer.h"

#include "Asset/AssetMenager.h"
#include "Asset/MaterialSerializer.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <format>

void Editor::OnAttach()
{
    AssetMenager::Get().DeserializeAssetRegistry();
    Renderer::Init();
    std::cout << "finish Init\n";

    m_Camera = CreateRef<Camera>(90.f);
    m_ActiveScene = CreateRef<Scene>();

    m_HierarhchyPanel = CreateScope<SceneHierarchyPanel>(m_ActiveScene);
    m_TimeLinePanel = CreateScope<TimeLinePanel>(m_ActiveScene);
    m_MaterialPanel = CreateScope<MaterialPanel>(nullptr);

    /*
    {
        MaterialInfo mat;
        mat.Albedo = glm::vec3(0.9f, 0.9f, 0.9f);
        mat.Roughness = 0.01;
        m_Materials->AddElement(mat);
    }
    */
}

void Editor::OnDetach()
{
    AssetMenager::Get().SerializeAssetRegistry();
    Renderer::ShutDown();
}

void Editor::OnUpdate(float DeltaTime)
{
    m_TimeLinePanel->OnUpdate(DeltaTime);

    if (Renderer::GetStatus() == RenderStatus::Free)
    {
        Renderer::ResizeImage(m_ImageWidth, m_ImageHeight);
        m_Camera->OnResize(m_ImageWidth, m_ImageHeight);
        Timer timer;
        Renderer::RenderPreview(m_ActiveScene, m_Camera, bAcumutale);
        if (bAcumutale)
        {
            m_LastTimeRender += timer.ElapsedMiliseconds();
            m_Pasess++;
        }
        else
        {
            m_LastTimeRender = timer.ElapsedMiliseconds();
            m_Pasess = 1;
        }
    }

    bool CameraMovement = false;
    if (bViewPortActive)
    {
        CameraMovement = m_Camera->OnUpdate(DeltaTime);
    }

    if (!bViewPortActive)
    {
        return;
    }

    if (CameraMovement)
    {
        return;
    }

    if (Input::IsKeyDown(GLFW_KEY_Q))
    {
        m_GizmoType = -1;
    }

    if (Input::IsKeyDown(GLFW_KEY_W))
    {
        m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
    }

    if (Input::IsKeyDown(GLFW_KEY_E))
    {
        m_GizmoType = ImGuizmo::OPERATION::ROTATE;
    }

    if (Input::IsKeyDown(GLFW_KEY_R))
    {
        m_GizmoType = ImGuizmo::OPERATION::SCALE;
    }
}

void Editor::OnUIRender()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Scene"))
        {
            if (ImGui::MenuItem("New")) 
            {
                NewScene();
            }

            if (ImGui::MenuItem("Open..."))
            {
                std::string path = FileDialog::OpenFile("Scne (*.yaml)\0*.yaml\0");
                std::cout << path << std::endl;
                if (!path.empty())
                {
                    NewScene();
                    SceneSerializer deserializer(m_ActiveScene);
                    deserializer.Deserialize(path);
                    m_ActiveScene->Update(0);
                }
            }

            if (ImGui::MenuItem("Save As..."))
            {
                std::string path = FileDialog::SaveFile("Scne (*.yaml)\0*.yaml\0");
                if (!path.empty())
                {
                    SceneSerializer serializer(m_ActiveScene);
                    serializer.Serialize(path);
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Render"))
        {
            if (ImGui::MenuItem("Render Image"))
            {
                //Timer timer;
                Renderer::RenderImage(m_ActiveScene, m_Camera);
                //float time = timer.ElapsedMiliseconds();
                //std::cout << "RenderTime " << time << std::endl;
            }

            if (ImGui::MenuItem("Render Animation"))
            {
                Renderer::RenderVideo(m_ActiveScene, m_Camera);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window"))
        {
            if (ImGui::MenuItem("Deatis"))
            {
                m_Open = true;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Assets"))
        {
            if (ImGui::MenuItem("New Mesh"))
            {
                std::string path = FileDialog::OpenFile("Mesh (*.obj)\0*.obj\0");
                if (!path.empty())
                {
                    AssetMenager::Get().ImportAsset(path);
                }
            }

            if (ImGui::MenuItem("New Material"))
            {
                m_MaterialModalOpen = true;
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    MaterialModal(m_MaterialModalOpen);

    m_HierarhchyPanel->OnImGuiRender();
    m_TimeLinePanel->OnImGuiRender();
    
    if (m_TimeLinePanel->GetCurentFrame() != m_Frame)
    {
        m_Frame = m_TimeLinePanel->GetCurentFrame();
        m_ActiveScene->Update(m_Frame);
    }

    if (m_MaterialPanel)
    {
        if (m_MaterialPanel->OnImGuiRender())
        {
            m_ActiveScene->UpdateMaterial(m_MaterialPanel->GetMaterial());
        }
    }
    
    if (m_Open)
    {
        ImGui::Begin("Settings", &m_Open);
        {
            RenderSetings setings = Renderer::GetSetings();
            bool bChange = false;
            bChange |= ImGui::DragScalar("Width", ImGuiDataType_U32, &setings.Width);
            bChange |= ImGui::DragScalar("Height", ImGuiDataType_U32, &setings.Height);
            bChange |= ImGui::DragInt("Ray Per Pixel", &setings.RaysperPixels);
            if (bChange)
            {
                Renderer::UpdateSetings(setings);
            }
        }
        ImGui::Separator();
        ImGui::Text("Last render: %.3f ms", m_LastTimeRender);
        ImGui::Checkbox("Acumulate", &bAcumutale);
        ImGui::DragInt("VP width", &m_ImageWidth);
        ImGui::DragInt("VP height", &m_ImageHeight);
        ImGui::DragInt("Passes", &m_Pasess);
        if (ImGui::Button("save"))
        {
            Renderer::SaveImage("Preview.bmp");
        }
        ImGui::Separator();
        glm::vec3 pos = m_Camera->GetLocation();
        ImGui::DragFloat3("Camera Pos", &pos.x);
        glm::vec3 Rotation = m_Camera->GetRotation();
        ImGui::DragFloat3("Camera Rot", &Rotation.x);
        ImGui::DragFloat("FOV", &m_CameraFOV, 1.0f, 10.f, 170.f);
        m_Camera->SetFOV(m_CameraFOV);
        ImGui::End();
    }


    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
    ImGui::Begin("Viewport");
    bViewPortActive = ImGui::IsWindowFocused();
    auto ViwportSize = ImGui::GetContentRegionAvail();
    m_ImageWidth = ViwportSize.x;
    m_ImageHeight = ViwportSize.y;
    uint32_t textureID = Renderer::GetImageID();
    ImGui::Image(reinterpret_cast<void*>(textureID), ViwportSize, ImVec2{ 0,1 }, ImVec2{ 1,0 });

    //Gizmos
    Object* objectRef = m_HierarhchyPanel->GetSelected();
    if (objectRef && m_GizmoType >= 0)
    {

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        float windowWidth = ImGui::GetWindowWidth();
        float windowHeight = ImGui::GetWindowHeight();

        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
        glm::mat4 cameraView = m_Camera->GetView();
        glm::mat4 projection = m_Camera->GetProjection();

        glm::mat4 transform = objectRef->GetTransform();

        ImGuizmo::MODE mode = m_GizmoType == ImGuizmo::OPERATION::SCALE ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

        ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(projection),
            (ImGuizmo::OPERATION)m_GizmoType, mode, glm::value_ptr(transform));

        if (ImGuizmo::IsUsing())
        {
            objectRef->SetTransform(transform);
            m_ActiveScene->Upload();
        }
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

void Editor::NewScene()
{
    m_ActiveScene = CreateRef<Scene>();
    m_HierarhchyPanel->SetContext(m_ActiveScene);
    m_TimeLinePanel->SetContext(m_ActiveScene);
    m_TimeLinePanel->SetFrame(0);
    m_ActiveScene->Upload();
}

void Editor::MaterialModal(bool& isOpen)
{
    if (isOpen)
    {
        ImGui::OpenPopup("Material Name");
    }

    //Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Material Name", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static std::string name("Material");
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strcpy_s(buffer, sizeof(buffer), name.c_str());

        ImGui::PushItemWidth(-1);
        if (ImGui::InputText("##Label", buffer, sizeof(buffer)))
        {
            name = std::string(buffer);
        }
        ImGui::PopItemWidth();

        if (ImGui::Button("Create", ImVec2(120, 0))) 
        {
            ImGui::CloseCurrentPopup();
            isOpen = false;

            auto path = AssetMenager::Get().GetBasePath();
            path /= "Material.mat";
            
            path.replace_filename(std::string(buffer));
            path.replace_extension(".mat");

            Ref<Material> mat = AssetMenager::Get().CreateAsset<Material>(path);
            MaterialSerializer serializer(mat);
            serializer.Serialize(path.generic_string());

            m_MaterialPanel = std::make_unique<MaterialPanel>(mat);
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup(); 
            isOpen = false; 
        }
        ImGui::EndPopup();
    }
}
