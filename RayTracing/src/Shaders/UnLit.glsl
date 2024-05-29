#version 460 core

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D u_Texture;

const float FLT_MAX = 3.402823466e+38;
const float PI = 3.1415926535;

struct Sphere
{
    vec3 Origin;
    float Radius;
    vec3 padding;
    int MaterialID;
};

struct Material
{
    vec3 Albedo;
    float Roughness;
};

uniform uint SphereSize;
layout(std430, binding = 1) buffer b_spheres
{
    Sphere spheres[];
};

uniform uint MatSize;
layout(std430, binding = 2) buffer b_materials
{
    Material materials[];
};

uniform vec2 Resolution;// potencjalnie do usuniêcia
uniform vec3 RayOrg;
uniform mat4 InvP;
uniform mat4 InvV;

//--------------------Utylis Functions
uint rngSeed;

uint PCGHash()
{
    rngSeed = rngSeed * 747796405u + 2891336453u;
    uint state = rngSeed;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float rand() //random float from 0 to 1
{
    return PCGHash() / float(0xFFFFFFFFU);
}

float randNormalDistribution() // potencilay slow function
{
    float theta = 2 * PI * rand();
    float rho = sqrt(-2 * log(rand()));
    return rho * cos(theta);
}

vec3 randDirection()
{
    float x = randNormalDistribution();
    float y = randNormalDistribution();
    float z = randNormalDistribution();
    return normalize(vec3(x, y, z));
}

vec3 randDirectionHemisphere(vec3 normal)
{
    vec3 dir = randDirection();
    return dir * sign(dot(normal, dir));
}

//--------------------Ray Tracing
struct Ray
{
    vec3 Origin;
    vec3 Direction;
};

struct HitInfo
{
    float HitDistance;
    vec3 HitPosition;
    vec3 normal;

    int ObjectIndex;
};

HitInfo TraceRay(Ray ray)
{
    HitInfo payload;
    payload.HitDistance = FLT_MAX;
    payload.ObjectIndex = -1;

    for (int i = 0; i < SphereSize; i++)
    {
        vec3 origin = ray.Origin - spheres[i].Origin;

        float a = dot(ray.Direction, ray.Direction);
        float b = 2.0f * dot(origin, ray.Direction);
        float c = dot(origin, origin) - spheres[i].Radius * spheres[i].Radius;

        float delta = b * b - (4.0 * a * c);

        //ray dont hit sphere
        if (delta < 0.0f)
        {
            continue;
        }

        //hited sphere is back
        float t1 = (-b - sqrt(delta)) / (2.0f * a);

        if (0 < t1 && t1 < payload.HitDistance)
        {
            payload.HitDistance = t1;
            payload.ObjectIndex = i;
        }
    }

    if (payload.ObjectIndex < 0)
        return payload;

    Sphere s = spheres[payload.ObjectIndex];

    vec3 origin = ray.Origin - s.Origin;
    payload.HitPosition = origin + ray.Direction * payload.HitDistance;
    payload.normal = normalize(payload.HitPosition);
    payload.HitPosition += s.Origin;

    return payload;
}

vec3 PixelColor(Ray ray)
{
    int bounces = 5; //convert to uniform
    vec3 finalColor = vec3(0.0f);
    float mul = 1.0f;

    for (int i = 0; i < bounces; i++)
    {
        HitInfo hit = TraceRay(ray);

        if (hit.ObjectIndex < 0)
        {
            vec3 skycolor = vec3(0.6f, 0.7f, 0.9f);
            //vec3 skycolor = vec3(0.0f);
            finalColor += skycolor * mul;
            break;
        }

        //vec3 LightDir = normalize(vec3(-1.0f, -1.0f, -1.0f));
        //float LightIntensity = max(dot(hit.normal, -LightDir), 0.0f);

        int matId = spheres[hit.ObjectIndex].MaterialID;
        Material mat = materials[matId];

        vec3 color = mat.Albedo;
        //color *= LightIntensity;

        finalColor += color * mul;
        mul *= 0.5f;

        //update ray for new trace
        ray.Origin = hit.HitPosition + hit.normal * 0.0001f;
        vec3 diffuseDir = randDirectionHemisphere(hit.normal);
        vec3 specularDir = reflect(ray.Direction, hit.normal);
        ray.Direction = mix(specularDir, diffuseDir, mat.Roughness);
    }

    return finalColor;
}

//--------------------Program
void main()
{
    vec2 id = vec2(gl_GlobalInvocationID.xy);
    vec2 uv = id / vec2(imageSize(u_Texture));
    uv = uv * 2.0f - 1.0f; // normalize uv to -1, 1 space

    rngSeed = uint(id.y * imageSize(u_Texture).x + id.x);

    vec4 target = InvP * vec4(uv, -1.0f, 1.0f);
    vec3 RayDir = vec3(InvV * vec4(normalize(vec3(target) / target.w), 0));

    Ray ray;
    ray.Origin = RayOrg;
    ray.Direction = RayDir;

    vec3 finalColor = vec3(0.0f);

    int RaysPerPixel = 1;
    for (int i = 0; i < RaysPerPixel; i++)
    {
        finalColor += PixelColor(ray);
    }

    finalColor /= float(RaysPerPixel);

    imageStore(u_Texture, ivec2(id), vec4(finalColor, 1.0f));
}