#pragma once

#ifndef __EPIC_SHADER__
#define __EPIC_SHADER__

#include "common/Rendering/Shaders/ShaderProgram.h"

class EpicShader : public ShaderProgram  
{
public:
    EpicShader(const std::unordered_map<GLenum, std::string>& inputShaders, GLenum lightingStage);
    virtual ~EpicShader();


    virtual void SetupShaderLighting(const class Light* light) const;
    virtual void SetupShaderMaterials() const;
    virtual void SetupShaderCamera(const class Camera* camera) const;

    void SetMetallic(float inValue);
    void SetRoughness(float inValue);
    void SetSpecular(float inValue);

    struct TextureSlots
    {
        enum Type
        {
            DIFFUSE = 0,
            NORMAL,
            DISPLACEMENT
        };
    };

    /*! \brief Stores the texture internally. Does not copy to OpenGL.
    *  \param slot The texture unit that we will be binding the texture to.
    *  \param inputTexture A pointer to the assignment framework's representation of a texture.
    */
    virtual void SetTexture(TextureSlots::Type slot, std::shared_ptr<class Texture> inputTexture);

    virtual void SetMaxDisplacement(float input);

    static std::unique_ptr<struct EpicLightingProperties> CreateLightProperties();
private:
    // Material Parameters
    virtual void UpdateMaterialBlock() const;
    float metallic;
    float roughness;
    float specular;

    // Material Bindings into the Shader
    static std::array<const char*, 3> MATERIAL_PROPERTY_NAMES;
    static const int MATERIAL_BINDING_POINT;
    GLuint materialBlockLocation;
    GLint materialBlockSize;
    std::array<GLuint, 3> materialIndices;
    std::array<GLint, 3> materialOffsets;
    GLuint materialBuffer;
    std::vector<GLubyte> materialStorage;

    GLenum lightingShaderStage;

    // Attenuation Uniform Handling
    virtual void UpdateAttenuationUniforms(const class Light* light) const;

    // Textures
    std::unordered_map<TextureSlots::Type, std::shared_ptr<class Texture>, std::hash<int> > textureSlotMapping;
    float maxDisplacement;
};

#endif