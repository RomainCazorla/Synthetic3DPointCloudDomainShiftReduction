#include "CheckCollisions.h"

bool CheckCollisions::OnReportContact(const chrono::ChVector<>& pA,
    const chrono::ChVector<>& pB,
    const chrono::ChMatrix33<>& plane_coord,
    const double& distance,
    const double& eff_radius,
    const chrono::ChVector<>& react_forces,
    const chrono::ChVector<>& react_torques,
    chrono::ChContactable* modA,
    chrono::ChContactable* modB)
{
    chrono::ChMatrix33<>& mplanecoord = const_cast<chrono::ChMatrix33<>&>(plane_coord);
    chrono::ChVector<> v1 = pA;
    chrono::ChVector<> v2;
    chrono::ChVector<> vn = mplanecoord.Get_A_Xaxis();
    if (distance < -0.05)
    {
        contacts[modA] = true;
        contacts[modB] = true;
        linkedContactable[modA] = modB;
        linkedContactable[modB] = modA;
    }
    return true;
}
