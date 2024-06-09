#version 460 core

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D u_Texture;

const float FLT_MAX = 3.402823466e+38;
const float PI = 3.1415926535;

//Triangle Data
struct Triangle
{
    ivec3 Vertexis;
    int ObjectID; //padding
};

layout(std430, binding = 1) restrict readonly buffer TrianglesBuff
{
    Triangle triangles[];
};

//Vartax Data
struct Vertex
{
    vec3 Position;
    float padding;
};

layout(std430, binding = 2) restrict readonly buffer VerticesBuff
{
    Vertex vertices[];
};

struct Mesh
{
    vec3 Min;
    int StartFace;
    vec3 Max;
    int Size;
};

layout(std430, binding = 3) restrict readonly buffer MeshesBuff
{
    Mesh meshes[];
};

//Instance Data
struct Instance
{
    mat4 LocalToWorld;
    mat4 WorldToLocal;
    int MeshID;
    int MaterialID;
    ivec2 _Padding; //pading
};

uniform uint InstancesSize;
layout(std430, binding = 4) restrict readonly buffer InstanceBuff
{
    Instance instances[];
};

//Material Data
struct Material
{
    vec3 Albedo;
    float Roughness;
    vec3 EmissiveColor;
    float EmissivePower;
    vec3 SpecularColor;
    float SpecularChance;
    vec3 padding;
    float Metallic;
};

layout(std430, binding = 5) restrict readonly buffer MaterialBuff
{
    Material materials[];
};

//--------------------Uniforms
uniform vec3 RayOrg;
uniform mat4 InvP;
uniform mat4 InvV;
uniform uint frameNum;
uniform uint RaysPerPixel;

//--------------------Utylis Functions
struct HitInfo
{
    bool didHit;
    float HitDistance;
    vec3 HitPosition;
    vec3 normal;
    int ObjectIndex;
};

struct Ray
{
    vec3 Origin;
    vec3 Direction;
};

Ray WorldRayToLocal(Ray ray, mat4 Transform)
{
    return Ray((Transform * vec4(ray.Origin, 1.0)).xyz, (Transform * vec4(ray.Direction, 0.0)).xyz);
}

vec3 LocalToWorld(vec4 vec, mat4 Transform)
{
    return (Transform * vec).xyz;
}

HitInfo RayTriangle(Ray ray, Triangle T)
{
    vec3 verA = vertices[T.Vertexis.x].Position;
    vec3 verB = vertices[T.Vertexis.y].Position;
    vec3 verC = vertices[T.Vertexis.z].Position;

    /*
    vec3 edgeAB = verB - verA; // E
    vec3 edgeAC = verC - verA; // F
    vec3 normal = cross(edgeAB, edgeAC); 
    vec3 ao = ray.Origin - verA; // T
    vec3 dao = cross(ray.Direction, ao);

    float determinant = dot(-ray.Direction, normal); // | -D E F | = -D * (E x F)
    float invDet = 1 / determinant; // posibli devide by 0

    float t = dot(ao, normal) * invDet;
    float u = dot(edgeAC, dao) * invDet;
    float v = -dot(edgeAB, dao) * invDet;
    float w = 1 - (u + v);

    */

    vec3 edgeAB = verB - verA;
    vec3 edgeAC = verC - verA;
    vec3 pvec = cross(ray.Direction,edgeAC);
    float det = dot(edgeAB,pvec);

    float invDet = 1 / det;

    vec3 tvec = ray.Origin - verA;
    vec3 qvec = cross(tvec, edgeAB);

    float t = dot(edgeAC,qvec) * invDet;
    float u = dot(tvec, pvec) * invDet;
    float v = dot(ray.Direction, qvec) * invDet;
    float w = 1 - (u + v);

    HitInfo Hit;
    //init hit info
    Hit.didHit = det >= 1E-6 && t > 0 && u >= 0 && v >= 0 && w >= 0;
    Hit.HitDistance = t;
    Hit.HitPosition = ray.Origin + ray.Direction * t;
    Hit.normal = normalize(cross(edgeAB, edgeAC)); // curentli flat shading
    //Hit.ObjectIndex = T.ObjectID; // triangle ID

    return Hit;
}

bool BBoxIntersect(const vec3 boxMin, const vec3 boxMax, const Ray ray) 
{
    vec3 invDir = 1.0f / ray.Direction;
    vec3 tbot = invDir * (boxMin - ray.Origin);
    vec3 ttop = invDir * (boxMax - ray.Origin);
    vec3 tmin = min(ttop, tbot);
    vec3 tmax = max(ttop, tbot);
    vec2 t = max(tmin.xx, tmin.yz);
    float t0 = max(t.x, t.y);
    t = min(tmax.xx, tmax.yz);
    float t1 = min(t.x, t.y);
    return t1 > max(t0, 0.0);
}

//--------------------PBR
vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0f, 1.0f);
}

vec3 LessThan(vec3 f, float value)
{
    return vec3(
        (f.x < value) ? 1.0f : 0.0f,
        (f.y < value) ? 1.0f : 0.0f,
        (f.z < value) ? 1.0f : 0.0f);
}

vec3 LinearToSRGB(vec3 rgb)
{
    rgb = clamp(rgb, 0.0f, 1.0f);

    return mix(
        pow(rgb, vec3(1.0f / 2.4f)) * 1.055f - 0.055f,
        rgb * 12.92f,
        LessThan(rgb, 0.0031308f)
    );
}

//GGX
float D(float alpha, vec3 N, vec3 H)
{
    float numerator = pow(alpha, 2.0);
    float NdotH = dot(N, H);
    float denominator = PI * pow(pow(NdotH, 2.0) * (pow(alpha, 2.0) - 1.0) + 1.0, 2.0);
    return numerator / denominator;
}

float G1(float alpha, vec3 N, vec3 X)
{
    float numerator = dot(N, X);
    float k = alpha / 2.0;
    float denominator = dot(N, X) * (1.0 - k) + k;
    return numerator / denominator;
}

float G(float alpha, vec3 N, vec3 V, vec3 L)
{
    return G1(alpha, N, V) * G1(alpha, N, L);
}

vec3 F(vec3 F0, vec3 V, vec3 H)
{
    return F0 + (vec3(1.0) - F0) * pow(1 - dot(V, H), 5.0);
}

//--------------------Random
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

HitInfo TraceRay(Ray ray)
{
    HitInfo payload;
    payload.didHit = false;
    payload.HitDistance = FLT_MAX;
    payload.ObjectIndex = -1;

    //go over evry instances
    for (int i = 0; i < InstancesSize; i++)
    {
        Ray localRay = WorldRayToLocal(ray, instances[i].WorldToLocal);// transform ray to instance local space

        //getMesh
        Mesh mesh = meshes[instances[i].MeshID];
        if (!BBoxIntersect(mesh.Min, mesh.Max, localRay))
        {
            continue;
        }

        //go over evry triangle in base model
        for (int t = mesh.StartFace; t < mesh.StartFace + mesh.Size; t++)
        {
            HitInfo hitResults = RayTriangle(localRay, triangles[t]); // return hit in local space

            if (hitResults.didHit && hitResults.HitDistance < payload.HitDistance)
            {
                payload = hitResults;
                payload.ObjectIndex = i;
            }
        }
    }

    if (payload.didHit)
    {
        //transform position and normal to world cordinate
        payload.HitPosition = LocalToWorld(vec4(payload.HitPosition, 1.0), instances[payload.ObjectIndex].LocalToWorld);
        payload.normal = LocalToWorld(vec4(payload.normal, 0.0), instances[payload.ObjectIndex].LocalToWorld);
        payload.normal = normalize(payload.normal);
    }

    return payload;
}

vec3 PixelColor(Ray ray)
{
    int bounces = 4; //convert to uniform
    vec3 light = vec3(0.0f);
    vec3 contribution = vec3(1.0f);

    for (int i = 0; i < bounces; i++)
    {
        HitInfo hit = TraceRay(ray);

        if (hit.ObjectIndex < 0)
        {
            vec3 skycolor = vec3(0.6f, 0.7f, 0.9f);
            //vec3 skycolor = vec3(0.31f);
            //vec3 skycolor = vec3(0.06f);
            light += skycolor * contribution;
            break;
        }

        int matId = instances[hit.ObjectIndex].MaterialID;
        Material mat = materials[matId];

        //update ray for new trace
        ray.Origin = hit.HitPosition + hit.normal * 0.0001f;

        vec3 diffuseDir = normalize(hit.normal + randDirection());
        vec3 specularDir = reflect(ray.Direction, hit.normal);
        vec3 outDir = normalize(mix(specularDir, diffuseDir, mat.Roughness * mat.Roughness));

        //pbr
        vec3 F0 = vec3(0.04);
        F0 = mix(F0, mat.Albedo, mat.Metallic);

        vec3 L = outDir;
        vec3 V = -ray.Direction;
        vec3 N = hit.normal;
        vec3 H = normalize(V + L);
        
        vec3 Ks = F(F0, V, H);
        vec3 Kd = vec3(1.0) - Ks;
        Kd *= 1.0 - mat.Metallic;

        vec3 cookTorranceNum = G(0, N, V, L) * F(F0, V, H);//D(0, N, H);
        float cookTorranceDenom = 4.0 * dot(V, N) * dot(L, N);
        vec3 cookTorace = cookTorranceNum / cookTorranceDenom;

        vec3 color = Kd * mat.Albedo + Ks * cookTorace;
        //color += mat.EmissiveColor * mat.EmissivePower;

        //light calculation
        light += mat.EmissiveColor * mat.EmissivePower * contribution;
        contribution *= color;
        ray.Direction = L;
    }

    return light;
}

//--------------------Program
void main()
{
    vec2 id = vec2(gl_GlobalInvocationID.xy);
    rngSeed = uint(id.y * imageSize(u_Texture).x + id.x);
    rngSeed *= frameNum;

    vec2 jitter = vec2(rand(), rand()) - 0.5f;
    //vec2 jitter = vec2(0.f);
    vec2 uv = (id + jitter) / vec2(imageSize(u_Texture));
    uv = uv * 2.0f - 1.0f; // normalize uv to -1, 1 space


    vec4 target = InvP * vec4(uv, -1.0f, 1.0f);
    vec3 RayDir = vec3(InvV * vec4(normalize(vec3(target) / target.w), 0));

    //vec3 RayDir = vec3(uv, -1.f);

    Ray ray;
    ray.Origin = RayOrg;
    ray.Direction = RayDir;

    //HitInfo hit = TraceRay(ray);

    vec3 finalColor = vec3(0.0f, 0.0f, 0.0f);

    //finalColor = PixelColor(ray);
    for (uint i = 0; i < RaysPerPixel; i++)
    {
        finalColor += PixelColor(ray);
    }
    
    finalColor /= float(RaysPerPixel);

    //float c_exposure = 0.5f;
    //finalColor *= c_exposure;
    //finalColor = ACESFilm(finalColor);//Tone Maping
    //finalColor = LinearToSRGB(finalColor);


    vec3 oldColor = vec3(imageLoad(u_Texture, ivec2(id)));
    oldColor *= float(frameNum - 1);
    finalColor = (oldColor + finalColor) / float(frameNum);
    
    imageStore(u_Texture, ivec2(gl_GlobalInvocationID.xy), vec4(finalColor, 1.0f));
}