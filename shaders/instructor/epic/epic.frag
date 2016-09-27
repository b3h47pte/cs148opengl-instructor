#version 400

in vec4 fragmentColor;
in vec4 vertexWorldPosition;
in vec3 vertexWorldNormal;
in vec2 fragmentUV;
in vec3 vertexWorldTangent;
in vec3 vertexWorldBitangent;

out vec4 finalColor;

uniform InputMaterial {
    float metallic;
    float roughness;
    float specular;
} material;

struct InputLightProperties {
    vec4 lightColor;
    vec4 secondaryColor;
    
    vec4 lightPosition;
    vec4 directionToLight;
    float lightRadius;
};
uniform InputLightProperties lightData;
uniform vec4 cameraPosition;

uniform float constantAttenuation;
uniform float linearAttenuation;
uniform float quadraticAttenuation;

uniform sampler2D diffuseTexture;
uniform bool useDiffuseTexture;

uniform sampler2D normalTexture;
uniform bool useNormalTexture;

subroutine vec3 lightingSubroutine(vec4 worldPosition, vec3 tangentNormal, mat3 tbn);
subroutine uniform lightingSubroutine inputLightSubroutine;

const float M_PI = 3.14159265359;

vec3 AttenuateLight(vec3 originalColor, vec4 worldPosition)
{
    float lightDistance = length(lightData.lightPosition - worldPosition);
    
    float lightRadiusAttenuation = pow(clamp(1.f - pow(lightDistance / lightData.lightRadius, 4.f), 0.f, 1.f), 2.f);
    float distanceAttenuation = constantAttenuation + lightDistance * linearAttenuation + lightDistance * lightDistance * quadraticAttenuation;
    float totalAttenuation = lightRadiusAttenuation / distanceAttenuation;
    return totalAttenuation * originalColor;
}

vec3 Diffuse(vec3 diffuseColor)
{
    return diffuseColor / M_PI;
}

float D_GGX(float alpha, float NdH)
{
    float denom = NdH * NdH * (alpha * alpha - 1.f) + 1.f;
    return (alpha * alpha / (M_PI * denom * denom));
}

float G_Schlick_Helper(float NdV, float k)
{
    return NdV / (NdV * (1.f - k) + k);
}

float G_Schlick(float NdV, float NdL)
{
    float k = pow(material.roughness + 1.f, 2.f) / 8.f;
    return G_Schlick_Helper(NdV, k) * G_Schlick_Helper(NdL, k);
}

vec3 F_Schlick(vec3 specularColor, float VdH)
{
    float power = exp2((-5.55473 * VdH - 6.98316) * VdH);
    return specularColor + (1.f - specularColor) * power;
}

vec3 Specular(vec3 specularColor, float NdL, float NdV, float NdH, float VdH)
{
    float alpha = material.roughness * material.roughness;
    float D = D_GGX(alpha, NdH);
    float G = G_Schlick(NdV, NdL);
    vec3 F = F_Schlick(specularColor, VdH);
    return (D * F * G / (4.f * NdL * NdV));
}

vec3 Shading(vec4 worldPosition, vec3 tangentNormal, vec4 inputL, vec3 lightColor, bool doDiffuse, bool doSpec, mat3 tbn)
{
    // Normal to the surface
    vec3 N = normalize(tangentNormal);
    
    // Recalculate L in tangent space
    vec3 L = tbn * vec3(inputL);

    // Direction from the surface to the eye
    vec3 V = tbn * normalize(vec3(cameraPosition - worldPosition));

    // Direction of maximum highlights
    vec3 H = normalize(L + V);
    float NdL = clamp(dot(N, L), 0.f, 1.f);
    float NdV = max(dot(N, V), 1e-5);
    float NdH = clamp(dot(N, H), 0.f, 1.f);

    float VdH = clamp(dot(V, H), 0.f, 1.f);
   
    vec4 baseColor;
    if (useDiffuseTexture) {
        baseColor = texture(diffuseTexture, fragmentUV);
    } else {
        baseColor = fragmentColor;
    }
    
    vec3 matDiffuse = vec3((1.f - material.metallic) * baseColor);
    vec3 matSpecular = mix(0.08f * vec3(material.specular, material.specular, material.specular), vec3(baseColor), material.metallic);
   
    vec3 diffuseColor = doDiffuse ? Diffuse(matDiffuse) : vec3(0);
    vec3 specularColor = doSpec ? Specular(matSpecular, NdL, NdV, NdH, VdH) : vec3(0);
    return (lightColor * NdL * (diffuseColor + specularColor));
}

subroutine(lightingSubroutine) vec3 pointLightSubroutine(vec4 worldPosition, vec3 tangentNormal, mat3 tbn)
{
    // Direction from the surface to the light
    vec4 L = normalize(lightData.lightPosition - worldPosition);
    //return AttenuateLight(Shading(worldPosition, tangentNormal, L, vec3(lightData.lightColor), true, true, tbn), worldPosition);
    return Shading(worldPosition, tangentNormal, L, vec3(lightData.lightColor), true, true, tbn);
}

subroutine(lightingSubroutine) vec3 directionalLightSubroutine(vec4 worldPosition, vec3 tangentNormal, mat3 tbn)
{
    return vec3(0.f);
    return Shading(worldPosition, tangentNormal, lightData.directionToLight, vec3(lightData.lightColor), true, true, tbn);
}

subroutine(lightingSubroutine) vec3 hemisphereLightSubroutine(vec4 worldPosition, vec3 tangentNormal, mat3 tbn)
{
    return vec3(0.f);
    float NdL = dot(tangentNormal, tbn * vec3(lightData.directionToLight));
    float w = clamp(NdL * 0.5 + 0.5, 0.f, 1.f);
    vec3 lightColor = vec3(mix(lightData.secondaryColor, lightData.lightColor, w));
    return Shading(worldPosition, tangentNormal, vec4(tangentNormal, 0), lightColor, true, false, tbn);
}

subroutine(lightingSubroutine) vec3 globalLightSubroutine(vec4 worldPosition, vec3 tangentNormal, mat3 tbn)
{
    return vec3(0.f);
}

void main()
{
    mat3 worldToTangentMatrix = transpose(mat3(vertexWorldTangent, vertexWorldBitangent, normalize(vertexWorldNormal)));
    vec3 tangentN = useNormalTexture ? normalize(vec3(texture(normalTexture, fragmentUV)) * 2.f - 1.f) : normalize(worldToTangentMatrix * vertexWorldNormal);
    finalColor = vec4(inputLightSubroutine(vertexWorldPosition, tangentN, worldToTangentMatrix), 1.f);    
}
