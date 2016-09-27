#pragma once

#ifndef __DIRECTIONAL_LIGHT__
#define __DIRECTIONAL_LIGHT__

#include "common/Scene/Light/Light.h"

class DirectionalLight : public Light  
{
public:
    DirectionalLight(std::unique_ptr<struct LightProperties> inProperties);
    virtual ~DirectionalLight();

    virtual glm::vec4 GetForwardDirection() const;
};

#endif
