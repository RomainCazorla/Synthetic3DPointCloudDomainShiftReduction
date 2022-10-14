#pragma once
#include "Object.h"

/// <summary>
/// An object placed in the physics simulator scene
/// </summary>
class PlacedObject
{
public:
	/// <summary>
	/// Only constructor available for PlacedObject
	/// </summary>
	/// <param name="baseObject">Mesh representation of the placed object</param>
	/// <param name="scale">Scale of the object in the scene compared to the object mesh definition</param>
	/// <param name="collisionBody">Collision body used in the simulation</param>
	PlacedObject(Object baseObject, double scale, std::shared_ptr<chrono::ChBody> collisionBody);

public:
	/// <summary>
	/// Object definition
	/// </summary>
	Object BaseObject;
	/// <summary>
	/// Scale of the object, compared to the defining mesh size
	/// </summary>
	double Scale;
	/// <summary>
	/// The collision body representing the object in the physics simulator
	/// </summary>
	std::shared_ptr<chrono::ChBody> CollisionBody;
};

