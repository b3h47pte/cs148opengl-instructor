#pragma once

#ifndef __HEMISPHERE_LIGHT__
#define __HEMISPHERE_LIGHT__

#include "common/Scene/Light/Light.h"

class HemisphereLight : public Light {
public:
    HemisphereLight(std::unique_ptr<struct LightProperties> inProperties);
    virtual ~HemisphereLight();

    virtual glm::vec4 GetForwardDirection() const;
};

#endif
