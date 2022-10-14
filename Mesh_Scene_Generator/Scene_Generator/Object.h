#pragma once
#include <string>
#include <random>
#include "chrono/physics/ChSystemNSC.h"
#include "chrono/geometry/ChTriangleMeshConnected.h"

/// <summary>
/// Represent an object and enough information to randomly place it in the scene
/// </summary>
class Object
{
public:
	Object();
	Object(std::string name, std::string file, std::string wnids, double minScale, double maxScale, bool isLarge);

	/// <summary>
	/// Generate a random volume (m3) from the object MinVolume and MaxVolume
	/// </summary>
	/// <param name="randomEngine">The random engine to compute the volume</param>
	/// <returns>A random volume (m3)</returns>
	double GenerateRandomVolume(std::default_random_engine randomEngine);
	/// <summary>
	/// Compute a random scale to be used for a given mesh, work with volume (m3)
	/// </summary>
	/// <param name="randomEngine">Random engine used to determine a desired scale</param>
	/// <param name="objectMesh">The mesh to be scaled</param>
	/// <returns>A scale to be applied to the given mesh in order to obtain a target volume</returns>
	double ComputeWantedScale(std::default_random_engine randomEngine, std::shared_ptr<chrono::geometry::ChTriangleMeshConnected> objectMesh);
	/// <summary>
	/// Fix until the equivalent is implemented in chrono
	/// </summary>
	/// <param name="mesh">Mesh whose bounds are to be computed</param>
	/// <param name="xmin">Minimum x bound of the object(m)</param>
	/// <param name="xmax">Maximum x bound of the object(m)</param>
	/// <param name="ymin">Minimum y bound of the object(m)</param>
	/// <param name="ymax">Maximum y bound of the object(m)</param>
	/// <param name="zmin">Minimum z bound of the object(m)</param>
	/// <param name="zmax">Maximum z bound of the object(m)</param>
	void ComputeMeshBounds(std::shared_ptr<chrono::geometry::ChTriangleMeshConnected> mesh, double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax);
	/// <summary>
	/// Fix until mesh bounds computation for ChTriangle is implemented in chrono
	/// </summary>
	/// <param name="mesh">The mesh whose volume should be computed</param>
	/// <returns>A mesh volume (m3)</returns>
	double ComputeMeshVolume(std::shared_ptr<chrono::geometry::ChTriangleMeshConnected> mesh);
public:
	/// <summary>
	/// The name of the object
	/// </summary>
	std::string Name;
	/// <summary>
	/// The file containing the mesh definition of the object
	/// </summary>
	std::string AssociatedFile;
	/// <summary>
	/// Wnids of the object (see ShapeNet for more information)
	/// </summary>
	std::string Wnids;
	/// <summary>
	/// TODO : change for associated library instead, or better : forgo it for a complete and clean library use
	/// </summary>
	bool IsLargeObject;
	/// <summary>
	/// The minimum volume of the object in m3
	/// </summary>
	double MinVolume;
	/// <summary>
	/// Maximum volume of the object in m3
	/// </summary>
	double MaxVolume;
};

