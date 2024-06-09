#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D dest;

uniform vec2 Resolution;// potencjalnie do usuniêcia
uniform vec3 RayOrg;
uniform mat4 InvP;
uniform mat4 InvV;
uniform mat4 InvVP;

void main()
{
    vec2 id = vec2(gl_GlobalInvocationID.xy);
    vec2 uv = id / Resolution;
    uv = uv * 2.0f - 1.0f; // pixel cord from -1 to 1;

    //float AspectRatio = Resolution.x / Resolution.y;
    //uv.x *= AspectRatio;

    //camera variables
    //vec4 screanSpaceFar = vec4(uv, 1.0f, 1.0f);
    //vec4 screanSpaceNear = vec4(uv, 0.0f, 1.0f);
    //vec4 far = InvVP * screanSpaceFar;
    //vec3 far3 = vec3(far) / far.w;
    //vec4 near = InvVP * screanSpaceNear;
    //vec3 near3 = vec3(near) / near.w;
    //vec3 RayDir = near3 - far3;//normalize(far.xyz - near.xyz);

    vec4 target = InvP * vec4(uv, -1.0f, 1.0f);
    //target.w = 0.f;
    //vec3 RayDir = vec3(InvV * target);

    vec3 RayDir = vec3(InvV * vec4(normalize(vec3(target) / target.w), 0));
    //vec3 RayDir = vec3(uv, -1.f);
    RayDir = normalize(RayDir); //performance havy

    //vec3 RayDir = normalize(Forward + Right * uv.x + Up * uv.y);

    //sphere variables
    float Radius = 0.5f;
    vec3 SphereOrg = vec3(0.0f);


    float a = dot(RayDir, RayDir);
    float b = 2.0f * dot(RayOrg, RayDir);
    float c = dot(RayOrg, RayOrg) - Radius * Radius;

    float delta = b * b - (4.0 * a * c);

    vec3 color = vec3(0.03f, 0.03f, 0.03f);
    if (RayDir.y < 0)
    {
        color = vec3(0.01f);
    }

    if (delta < 0.0f)
    {
        imageStore(dest, ivec2(id), vec4(color, 1.0f));
        return;
    }

    //float t0 = (-b + sqrt(delta)) / (2.0f * a);
    float t1 = (-b - sqrt(delta)) / (2.0f * a); //always the smallest , beacouse a always pozitive
    //if (t1 < 0)
    //{
    //    imageStore(dest, ivec2(id), vec4(color, 1.0f));
    //    return;
    //}

    vec3 HitPoint = RayOrg + RayDir * t1;
    vec3 normal = normalize(HitPoint - SphereOrg);

    //global light
    vec3 LightDir = normalize(vec3(-1.0f, -1.0f, -1.0f));
    float d = dot(normal, -LightDir);


    color = vec3(1.0f, 0.0f, 1.0f);
    color *= d;
    //color = vec4(normal*0.5f + 0.5f, 1.0f);
    imageStore(dest, ivec2(id), vec4(color, 1.0f));
}