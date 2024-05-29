
#include "Core/Application.h"
#include "imgui/imgui.h"

#include "Editor.h"


int main()
{
    ApplicationSpecification Spec;
    Spec.Name = "RayTracing";

    Application* App = new Application(Spec);
    App->PushLayer<Editor>();

    App->Run();

    delete App;
    return 0;
}