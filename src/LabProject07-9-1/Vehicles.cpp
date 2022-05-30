#include "pch.hpp"
#include "Vehicles.hpp"

constexpr COLLISION_TAGS Vehicles::GetTag() const noexcept
{
    return COLLISION_TAGS::CAR;
}
