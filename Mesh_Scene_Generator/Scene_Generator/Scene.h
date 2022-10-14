#pragma once
#include "ObjectLibrary.h"
#include "PlacedObject.h"

class Scene
{
public:
	/// <summary>
	/// Empty constructor (which is really empty, think to add layout and ground before anything else)
	/// </summary>
	Scene();
	/// <summary>
	/// Add a layout contained in the layoutLibrary to the scene
	/// </summary>
	/// <param name="mphysicalSystem">The physical engine where the scene should be added</param>
	/// <param name="layoutLibrary">The library containing the different layouts</param>
	/// <param name="randomEngine">Random engine used to select a layout</param>
	/// <param name="addToSystem">If true, the layout will be added to the engine, else its informations are stocked as ScaneLayout</param>
	void AddLayout(chrono::ChSystemNSC& mphysicalSystem, ObjectLibrary layoutLibrary, std::default_random_engine& randomEngine, bool addToSystem);
	/// <summary>
	/// Place a random number of large objects in the scene
	/// </summary>
	/// <param name="mphysicalSystem">The physical engine which contains the scene where objects should be added</param>
	/// <param name="library">The library from which objects should be drawn</param>
	/// <param name="randomEngine">Random engine used to select the objects</param>
	/// <param name="maxNumberOfLargeObject">The maximum number of object which should be added</param>
	void AddLargeObjects(chrono::ChSystemNSC& mphysicalSystem, ObjectLibrary library, std::default_random_engine& randomEngine, int maxNumberOfLargeObject);
	/// <summary>
	/// Place a random number of small objects in the scene
	/// </summary>
	/// <param name="mphysicalSystem">The physical engine which contains the scene where objects should be added</param>
	/// <param name="library">The library from which objects should be drawn</param>
	/// <param name="randomEngine">Random engine used to select the objects</param>
	/// <param name="maxNumberOfLargeObject">The maximum number of object which should be added</param>
	/// <param name="placeOnLargeObject">If true, the small objects are placed on the bigger one (However, there is always a chance that they will fall during simulation)</param>
	void AddSmallObjects(chrono::ChSystemNSC& mphysicalSystem, ObjectLibrary library, std::default_random_engine& randomEngine, int maxNumberOfSmallObject, bool placeOnLargeObject);
	/// <summary>
	/// Add a ground to the scene, following the layout used. Useful when layout is added last
	/// </summary>
	void AddGround(chrono::ChSystemNSC& mphysicalSystem);
	/// <summary>
	/// Place a given object in the scene
	/// </summary>
	/// <param name="mphysicalSystem">The physical engine which contains the scene where the object should be added</param>
	/// <param name="object">The object to add</param>
	/// <param name="commonMaterial">Material associated to the object (contains object forces settings)</param>
	/// <param name="randomEngine">Random engine used to determine the object scale</param>
	/// <param name="position">Where to put the object</param>
	/// <param name="rotationAngle">Angle to be applied around the vertical axis (yaw). No yaw is applied by default (0.0)</param>
	/// <param name="useConvexHull">
	/// If true, a convex hull is used for collision computation. Some convex hull are badly defined by chrono, so it is not a good idea to use this mode.
	/// If false, use the full geometry for collision, which is more precise but need more computational power.
	/// False by default.
	/// </param>
	/// <param name="mass">Mass of the object during the simulation, 10.0 by default</param>
	/// <param name="fixed">If fixed, the object can't be moved by collision (eg : a wall), false by defaults</param>
	/// <param name="addToSystem">Will only be added to the physical engine if this parameter is set to yes, true by default.</param>
	/// <returns>The pointer directed to the object in a format which can be used by Chrono++</returns>
	std::shared_ptr<chrono::ChBody> AddObject(chrono::ChSystemNSC& mphysicalSystem, Object object, std::shared_ptr<chrono::ChMaterialSurfaceNSC>& commonMaterial,
		std::default_random_engine randomEngine, chrono::Vector position, double rotationAngle = 0.0, bool useConvexHull = false, double mass = 10.0, bool fixed = false, bool addToSystem = true);
	/// <summary>
	/// Return the total area (m²) contained in the scene
	/// </summary>
	double GetSceneArea();


public:
	/// <summary>
	/// Minimum points of the scene bounding box
	/// </summary>
	chrono::Vector SceneBoundingBoxMin;
	/// <summary>
	/// Maximum points of the scene bounding box
	/// </summary>
	chrono::Vector SceneBoundingBoxMax;
	/// <summary>
	/// Each of the placed large objects boundaries
	/// </summary>
	std::vector<std::tuple<chrono::Vector, chrono::Vector>> LargeObjectsBoundaries;
	/// <summary>
	/// The layout used in the scene
	/// </summary>
	Object UsedLayout;
	/// <summary>
	/// Objects used in the simulation
	/// </summary>
	std::vector<PlacedObject> MovingObjects;
	/// <summary>
	/// Pointer toward the scene layout as an object which can be used in the simulation
	/// </summary>
	std::shared_ptr<chrono::ChBody> SceneLayout;
};

