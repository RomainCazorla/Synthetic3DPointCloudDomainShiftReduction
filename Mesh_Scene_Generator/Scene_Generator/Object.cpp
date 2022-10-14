#include "Object.h"
#include <math.h>


Object::Object()
{
	Name = "";
	AssociatedFile = "";
	Wnids = "";
	MinVolume = 1.0;
	MaxVolume = 1.0;
	IsLargeObject = false;
}

Object::Object(std::string name, std::string file, std::string wnids, double minScale, double maxScale, bool isLarge)
{
	Name = name;
	AssociatedFile = file;
	Wnids = wnids;
	MinVolume = minScale;
	MaxVolume = maxScale;
	IsLargeObject = isLarge;
}


double Object::GenerateRandomVolume(std::default_random_engine randomEngine)
{
	std::uniform_real_distribution<double> uniformDistribution(MinVolume, MaxVolume);
	return uniformDistribution(randomEngine);
}


double Object::ComputeWantedScale(std::default_random_engine randomEngine, std::shared_ptr<chrono::geometry::ChTriangleMeshConnected> objectMesh)
{
	double wantedVolume = GenerateRandomVolume(randomEngine);
	double currentVolume = ComputeMeshVolume(objectMesh);

	std::cout << wantedVolume << " " << currentVolume << std::endl;

	return cbrt(wantedVolume / currentVolume);
}

void Object::ComputeMeshBounds(std::shared_ptr<chrono::geometry::ChTriangleMeshConnected> mesh, double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax)
{
	std::vector<chrono::ChVector<double>> vertices = mesh->getCoordsVertices();

	if (vertices.size() > 0)
	{
		xmin = vertices[0][0];
		xmax = vertices[0][0];
		ymin = vertices[1][0];
		ymax = vertices[1][0];
		zmin = vertices[2][0];
		zmax = vertices[2][0];


		for (int i = 1; i < vertices.size(); i++)
		{
			if (vertices[i][0] < xmin)
				xmin = vertices[i][0];
			else if (vertices[i][0] > xmax)
				xmax = vertices[i][0];
			if (vertices[i][1] < ymin)
				ymin = vertices[i][1];
			else if (vertices[i][1] > ymax)
				ymax = vertices[i][1];
			if (vertices[i][2] < zmin)
				zmin = vertices[i][2];
			else if (vertices[i][2] > zmax)
				zmax = vertices[i][2];
		}
	}
}

// Could be directly included in chrono
double Object::ComputeMeshVolume(std::shared_ptr<chrono::geometry::ChTriangleMeshConnected> mesh)
{
	double xmin, xmax, ymin, ymax, zmin, zmax;
	ComputeMeshBounds(mesh, xmin, xmax, ymin, ymax, zmin, zmax);
	std::cout << " " << xmin << " " << xmax << " " << ymin << " " << ymax << " " << zmin << " " << zmax << std::endl;
	double volume =  (xmax - xmin) * (ymax - ymin) * (zmax - zmin);
	return volume;
}