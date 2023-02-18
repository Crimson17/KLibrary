#pragma once

#include "math/vector/vector3.h"


namespace kl {
    struct sphere
    {
        float3 center = {};
        float radius = 0.0f;

        sphere();

        sphere(const float3& center, float radius);
    };

    std::ostream& operator<<(std::ostream& os, const sphere& obj);
}
