#include "instructor/Shaders/EpicShader.h"
#include "instructor/Light/EpicLightingProperties.h"
#include "common/Scene/Light/Light.h"
#include "common/Scene/Camera/Camera.h"
#include "common/Utility/Texture/TextureLoader.h"
#include "common/Rendering/Textures/Texture2D.h"

std::array<const char*, 3> EpicShader::MATERIAL_PROPERTY_NAMES = {
    "InputMaterial.metallic",
    "InputMaterial.roughness",
    "InputMaterial.specular"
};
const int EpicShader::MATERIAL_BINDING_POINT = 0;

EpicShader::EpicShader(const std::unordered_map<GLenum, std::string>& inputShaders, GLenum lightingStage) :
    ShaderProgram(inputShaders), materialBlockLocation(0), materialBlockSize(0), materialBuffer(0),
    lightingShaderStage(lightingStage), maxDisplacement(0.5f)
{
    if (!shaderProgram) {
        return;
    }

    SetupUniformBlock<3>("InputMaterial", MATERIAL_PROPERTY_NAMES, materialIndices, materialOffsets, materialStorage, materialBlockLocation, materialBlockSize, materialBuffer);
    UpdateMaterialBlock();
}

EpicShader::~EpicShader() 
{
    OGL_CALL(glDeleteBuffers(1, &materialBuffer));
}

void EpicShader::UpdateMaterialBlock() const 
{
    StartUseShader();

    memcpy((void*)(materialStorage.data() + materialOffsets[0]), &metallic, sizeof(float));
    memcpy((void*)(materialStorage.data() + materialOffsets[1]), &roughness, sizeof(float));
    memcpy((void*)(materialStorage.data() + materialOffsets[2]), &specular, sizeof(float));

    if (materialBuffer && materialBlockLocation != GL_INVALID_INDEX) {
        OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, materialBuffer));
        OGL_CALL(glBufferData(GL_UNIFORM_BUFFER, materialBlockSize, materialStorage.data(), GL_STATIC_DRAW));
        OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, MATERIAL_BINDING_POINT, materialBuffer));
        OGL_CALL(glUniformBlockBinding(shaderProgram, materialBlockLocation, MATERIAL_BINDING_POINT));
        OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    }

    StopUseShader();
}

void EpicShader::SetupShaderLighting(const class Light* light) const  
{
    if (!light) {
        SetShaderSubroutine("inputLightSubroutine", "globalLightSubroutine", lightingShaderStage);
    } else {
        const EpicLightingProperties* lightProperty = static_cast<const EpicLightingProperties*>(light->GetPropertiesRaw());
        SetShaderUniform("lightData.lightColor", lightProperty->lightColor);
        SetShaderUniform("lightData.secondaryColor", lightProperty->secondaryColor);
        SetShaderUniform("lightData.lightRadius", lightProperty->lightRadius);
        SetShaderUniform("lightData.lightPosition", light->GetPosition());
        SetShaderUniform("lightData.directionToLight", light->GetForwardDirection());

        switch (light->GetLightType())  
        {
        case Light::LightType::POINT:
            SetShaderSubroutine("inputLightSubroutine", "pointLightSubroutine", lightingShaderStage);
            break;
        case Light::LightType::DIRECTIONAL:
            SetShaderSubroutine("inputLightSubroutine", "directionalLightSubroutine", lightingShaderStage);
            break;
        case Light::LightType::HEMISPHERE:
            SetShaderSubroutine("inputLightSubroutine", "hemisphereLightSubroutine", lightingShaderStage);
            break;
        default:
            std::cerr << "WARNING: Specified Light Type Not Supported -- " << static_cast<int>(light->GetLightType()) << std::endl;
            SetShaderSubroutine("inputLightSubroutine", "globalLightSubroutine", lightingShaderStage);
            break;
        }
    }

    UpdateAttenuationUniforms(light);
}

void EpicShader::UpdateAttenuationUniforms(const Light* light) const {
    float constant = 1.f, linear = 0.f, quadratic = 0.f;
    if (light) {
        light->GetAttenuation(constant, linear, quadratic);
    }

    SetShaderUniform("constantAttenuation", constant);
    SetShaderUniform("linearAttenuation", linear);
    SetShaderUniform("quadraticAttenuation", quadratic);
}

void EpicShader::SetupShaderMaterials() const  
{
    // Need to make sure the material buffer is bound.
    OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, MATERIAL_BINDING_POINT, materialBuffer));

    // Make sure the right textures are bound.
    if (textureSlotMapping.find(TextureSlots::DIFFUSE) != textureSlotMapping.end()) {
        const Texture* diffuseTexture = textureSlotMapping.at(TextureSlots::DIFFUSE).get();
        diffuseTexture->BeginRender(static_cast<int>(TextureSlots::DIFFUSE));
        SetShaderUniform("useDiffuseTexture", (int)true);
    } else {
        SetShaderUniform("useDiffuseTexture", (int)false);
    }

    if (textureSlotMapping.find(TextureSlots::NORMAL) != textureSlotMapping.end()) {
        const Texture* normalTexture = textureSlotMapping.at(TextureSlots::NORMAL).get();
        normalTexture->BeginRender(static_cast<int>(TextureSlots::NORMAL));
        SetShaderUniform("useNormalTexture", (int)true);
    } else {
        SetShaderUniform("useNormalTexture", (int)false);
    }

    if (textureSlotMapping.find(TextureSlots::DISPLACEMENT) != textureSlotMapping.end()) {
        const Texture* displacementTexture = textureSlotMapping.at(TextureSlots::DISPLACEMENT).get();
        displacementTexture->BeginRender(static_cast<int>(TextureSlots::DISPLACEMENT));
        SetShaderUniform("useDisplacementTexture", (int)true);
    } else {
        SetShaderUniform("useDisplacementTexture", (int)false);
    }

    // While we're here, also setup the textures too.
    SetShaderUniform("diffuseTexture", static_cast<int>(TextureSlots::DIFFUSE));
    SetShaderUniform("normalTexture", static_cast<int>(TextureSlots::NORMAL));
    SetShaderUniform("displacementTexture", static_cast<int>(TextureSlots::DISPLACEMENT));
    SetShaderUniform("maxDisplacement", maxDisplacement);
}

void EpicShader::SetupShaderCamera(const class Camera* camera) const  
{
    SetShaderUniform("cameraPosition", camera->GetPosition());
}

std::unique_ptr<EpicLightingProperties> EpicShader::CreateLightProperties()
{
    return make_unique<EpicLightingProperties>();
}

void EpicShader::SetMetallic(float inValue)
{
    metallic = inValue;
    UpdateMaterialBlock();
}

void EpicShader::SetRoughness(float inValue)
{
    roughness = inValue;
    UpdateMaterialBlock();
}

void EpicShader::SetSpecular(float inValue)
{
    specular = inValue;
    UpdateMaterialBlock();
}

void EpicShader::SetTexture(TextureSlots::Type slot, std::shared_ptr<class Texture> inputTexture)
{
    textureSlotMapping[slot] = std::move(inputTexture);
}

void EpicShader::SetMaxDisplacement(float input)
{
    maxDisplacement = input;
}
