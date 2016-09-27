#include "instructor/Light/HemisphereLight.h"
#include "common/Scene/Light/LightProperties.h"

HemisphereLight::HemisphereLight(std::unique_ptr<struct LightProperties> inProperties) :
    Light(std::move(inProperties), Light::LightType::HEMISPHERE) 
{
}

HemisphereLight::~HemisphereLight() 
{
}

glm::vec4 HemisphereLight::GetForwardDirection() const  
{
    return glm::vec4(0.f, 1.f, 0.f, 0.f);
}