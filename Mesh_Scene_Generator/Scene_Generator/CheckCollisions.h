#pragma once
#define NOMINMAX

#include "chrono/physics/ChContactContainer.h"
#include<map>

using namespace chrono;

/// <summary>
/// Class used to work with collisions between the object present in the scene
/// </summary>
class CheckCollisions : public ChContactContainer::ReportContactCallback
{
public:
    virtual bool OnReportContact(const ChVector<>& pA,
        const ChVector<>& pB,
        const ChMatrix33<>& plane_coord,
        const double& distance,
        const double& eff_radius,
        const ChVector<>& react_forces,
        const ChVector<>& react_torques,
        ChContactable* modA,
        ChContactable* modB) override;

    std::map<chrono::ChContactable*, bool> contacts;
    std::map<chrono::ChContactable*, chrono::ChContactable*> linkedContactable;
};

