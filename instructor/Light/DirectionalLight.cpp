#include "instructor/Light/DirectionalLight.h"
#include "common/Scene/Light/LightProperties.h"

DirectionalLight::DirectionalLight(std::unique_ptr<struct LightProperties> inProperties):
    Light(std::move(inProperties), Light::LightType::DIRECTIONAL)
{
}

DirectionalLight::~DirectionalLight()  
{
}

glm::vec4 DirectionalLight::GetForwardDirection() const
{
    return -1.f * Light::GetForwardDirection();
}