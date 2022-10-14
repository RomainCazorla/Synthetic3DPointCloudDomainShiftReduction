#include "PlacedObject.h"

PlacedObject::PlacedObject(Object baseObject, double scale, std::shared_ptr<chrono::ChBody> collisionBody) : BaseObject(baseObject)
{
	Scale = scale;
	CollisionBody = collisionBody;
}