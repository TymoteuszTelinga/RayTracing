#pragma once

#include "Scene.h"
#include "Image.h"
#include "Core/Base.h"
#include "Camera.h"
#include "Program.h"

struct RenderSetings
{
	uint32_t Width = 1500;
	uint32_t Height = 800;
	int Bounces = 10;
	int RaysperPixels = 300;
	int FrameRate = 30;
	std::string fileName = "test";
};

enum class RenderStatus
{
	Rendering = 0,
	Free
};

class Renderer
{
public:
	static void Init();
	static void ShutDown();

	static void UpdateSetings(RenderSetings& setings);
	static void ResizeImage(uint32_t Width, uint32_t Height);
	static void RenderPreview(const Ref<Scene>& scene, Ref<Camera>& camera, bool acumulate);
	static bool RenderImage(const Ref<Scene>& scene, Ref<Camera>& camera, bool save = true);
	static bool RenderVideo(const Ref<Scene>& scene, Ref<Camera>& camera);
	static void SaveImage(const std::string& filepath);
	static void SaveImage();
	static uint32_t GetImageID();
	static RenderStatus GetStatus() { return s_Status; };
	static RenderSetings GetSetings() { return s_Setings; };

private:
	static Image* s_Image;
	static Program* s_Shader;
	static RenderSetings s_Setings;
	static RenderStatus s_Status;
};