#include "math/ray/plane.h"

#include "math/math.h"


// Class
kl::plane::plane()
{}

kl::plane::plane(const float3& origin, const float3& normal)
    : origin(origin), normal(math::normalize(normal))
{}

bool kl::plane::is_in_front(const float3& point) const
{
    const float result = (point - origin) * normal;
    return (result >= 0.0f);
}

// Format
std::ostream& kl::operator<<(std::ostream& os, const kl::plane& obj)
{
    os << "{" << obj.origin << ", " << obj.normal << "}";
    return os;
}