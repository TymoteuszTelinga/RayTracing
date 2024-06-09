#include "Renderer.h"

#include <JoMPEG/jo_mpeg.h>
#include <iostream>
#include "Core/Timer.h"
#include "Core/FileDialog.h"

Image* Renderer::s_Image(nullptr);
Program* Renderer::s_Shader(nullptr);
RenderSetings Renderer::s_Setings;
RenderStatus Renderer::s_Status = RenderStatus::Free;

void Renderer::Init()
{
    std::cout << "start Init Renderer\n";
	s_Image = new Image(720,720);
	s_Shader = new Program("src\\Shaders\\triangle.glsl");
}

void Renderer::ShutDown()
{
    delete s_Image;
    delete s_Shader;
}

void Renderer::UpdateSetings(RenderSetings& setings)
{
    s_Setings = setings;
}

void Renderer::ResizeImage(uint32_t Width, uint32_t Height)
{
    if (s_Image->GetWidth() != Width || s_Image->GetHeight() != Height)
    {
	    s_Image->Resize(Width, Height);
    }
}

void Renderer::RenderPreview(const Ref<Scene>& scene, Ref<Camera>& camera, bool acumulate)
{
    static int frame = 1;
    if (acumulate)
    {
        frame++;
    }
    else
    {
        frame = 1;
    }

    glm::vec3 CameraPos = camera->GetLocation();
    s_Shader->Bind();

    s_Shader->SetUniform3f("RayOrg", CameraPos.x, CameraPos.y, CameraPos.z);
    s_Shader->SetUniformMat4f("InvP", camera->GetInverseProjection());
    s_Shader->SetUniformMat4f("InvV", camera->GetInversView());
    s_Shader->SetUniform1ui("InstancesSize", scene->GetObjectsCount());
    s_Shader->SetUniform1ui("frameNum", frame);
    s_Shader->SetUniform1ui("RaysPerPixel", 3);

    s_Shader->Dispatch(ceil(s_Image->GetWidth() / 8)+1, ceil(s_Image->GetHeight() / 4)+1);
    s_Status = RenderStatus::Free;
}

bool Renderer::RenderImage(const Ref<Scene>& scene, Ref<Camera>& camera, bool save)
{
    s_Status = RenderStatus::Rendering;

    glm::vec3 CameraPos = camera->GetLocation();
    s_Image->Resize(s_Setings.Width, s_Setings.Height);
    camera->OnResize(s_Setings.Width, s_Setings.Height);

    float TotalTime = 0.0f;
    for (size_t i = 1; i < s_Setings.RaysperPixels; i++)
    {
        Timer t;
        s_Shader->Bind();
        s_Shader->SetUniform3f("RayOrg", CameraPos.x, CameraPos.y, CameraPos.z);
        s_Shader->SetUniformMat4f("InvP", camera->GetInverseProjection());
        s_Shader->SetUniformMat4f("InvV", camera->GetInversView());
        s_Shader->SetUniform1ui("InstancesSize", scene->GetObjectsCount());
        s_Shader->SetUniform1ui("frameNum", i);
        s_Shader->SetUniform1ui("RaysPerPixel", 2);

        s_Shader->Dispatch(ceil((s_Image->GetWidth() / 8)+1), ceil((s_Image->GetHeight() / 4)+1));
        TotalTime += t.ElapsedMiliseconds();
    }

    if (save)
    {
        s_Image->Save(s_Setings.fileName+".bmp");
        std::cout << "Total time(ms): " << TotalTime << std::endl;
        s_Status = RenderStatus::Free;
    }
    return true;
}

bool Renderer::RenderVideo(const Ref<Scene>& scene, Ref<Camera>& camera)
{
    s_Status = RenderStatus::Rendering;

    s_Image->Resize(s_Setings.Width, s_Setings.Height);
    camera->OnResize(s_Setings.Width, s_Setings.Height);
    
    FILE* fp = fopen("vid.mpeg", "wb");

    for (uint32_t i = 0; i < scene->GetSceneDuration(); i++)
    {
        scene->Update(i);
        //RenderPreview(scene, camera);
        RenderImage(scene, camera, false);
        uint8_t* Pixels = s_Image->GetImageData();
        jo_write_mpeg(fp, Pixels, s_Setings.Width, s_Setings.Height, s_Setings.FrameRate);
        delete[] Pixels;
        std::cout << "Frame " << i << std::endl;
    }

    fclose(fp);
    s_Status = RenderStatus::Free;
    return true;
}

void Renderer::SaveImage(const std::string& filepath)
{
    s_Image->Save(filepath);
}

void Renderer::SaveImage()
{
    std::string path = FileDialog::SaveFile("Image (*.bmp)\n*.bmp\n");
    if (path.empty())
    {
        path = s_Setings.fileName + ".bmp";
    }
    s_Image->Save(path);
}

uint32_t Renderer::GetImageID()
{
	return s_Image->GetRendererID();
}
