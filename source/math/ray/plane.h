#pragma once

#include "math/vector/vector3.h"


namespace kl {
    struct plane
    {
        float3 normal = {};
        float3 point = {};

        plane();

        plane(const float3& normal, const float3& point);
    };

    std::ostream& operator<<(std::ostream& os, const plane& obj);
}
