#pragma once

#ifndef __EPIC_LIGHT_PROPERTIES__
#define __EPIC_LIGHT_PROPERTIES__

#include "common/Scene/Light/LightProperties.h"

struct EpicLightingProperties : public LightProperties {
    glm::vec4 lightColor;
    glm::vec4 secondaryColor;
    float lightRadius;
};

#endif
