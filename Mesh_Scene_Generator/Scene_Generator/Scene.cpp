#define _USE_MATH_DEFINES
#include <math.h>

#include "Scene.h"
#include "chrono/assets/ChTexture.h"
#include "chrono/assets/ChTriangleMeshShape.h"
#include "chrono/physics/ChBodyEasy.h"
#include "chrono/assets/ChBoxShape.h"


Scene::Scene()
{

}

void Scene::AddLayout(chrono::ChSystemNSC& mphysicalSystem, ObjectLibrary layoutLibrary, std::default_random_engine& randomEngine, bool addToSystem)
{
    auto mat = chrono_types::make_shared<chrono::ChMaterialSurfaceNSC>();
    mat->SetFriction(0.4f);
    mat->SetCompliance(0.0);
    mat->SetComplianceT(0.0);
    mat->SetDampingF(1.0f);

    

    UsedLayout = layoutLibrary.GiveRandomObject(randomEngine, true);

    // For testing purposes, this is a good layout
    // Object fixedLayout(std::string("scene"), std::string(layoutLibrary.LibraryRoot + "office9_layout.obj"), std::string("000"), 1.0, 1.0, true);
    // UsedLayout = fixedLayout;
    SceneLayout = AddObject(mphysicalSystem, UsedLayout, mat, randomEngine, chrono::Vector(0, 0, 0), 0.0, false, 10.0, true, addToSystem);

    SceneLayout->GetTotalAABB(SceneBoundingBoxMin, SceneBoundingBoxMax);
}

void Scene::AddLargeObjects(chrono::ChSystemNSC& mphysicalSystem, ObjectLibrary library, std::default_random_engine& randomEngine, int maxNumberOfLargeObject)
{
    // TODO : check if useful to keep it here or keep a constant object
    // Generate the common material for the scene object
    auto mat = chrono_types::make_shared<chrono::ChMaterialSurfaceNSC>();
    mat->SetFriction(0.5f);
    mat->SetRollingFriction(0.5f);
    mat->SetSpinningFriction(0.5f);
    mat->SetCompliance(0.0);
    mat->SetComplianceT(0.0);
    mat->SetDampingF(1.0f);

    std::uniform_real_distribution<double> randomPositionX(SceneBoundingBoxMin[0] + 0.1, SceneBoundingBoxMax[0] - 0.1);
    std::uniform_real_distribution<double> randomPositionY(SceneBoundingBoxMin[1] + 0.1, SceneBoundingBoxMax[1] - 0.1);
    std::uniform_real_distribution<double> randomPositionZ(SceneBoundingBoxMin[2] + 0.1, SceneBoundingBoxMax[2] - 0.1);
    bool useConvex = false;

    std::uniform_int_distribution<int> randomNumberOfLargeObject(std::min(5, maxNumberOfLargeObject), maxNumberOfLargeObject);
    std::uniform_real_distribution<double> randomAngleGiver(-M_PI, M_PI);

    int numberOfLargeObjects = randomNumberOfLargeObject(randomEngine);
    for (int i = 0; i < numberOfLargeObjects; i++)
    {
        chrono::Vector position(randomPositionX(randomEngine), randomPositionY(randomEngine), randomPositionZ(randomEngine));
        std::shared_ptr<chrono::ChBody> addedObject = AddObject(mphysicalSystem, library.GiveRandomObject(randomEngine, true), mat, randomEngine, position, randomAngleGiver(randomEngine), useConvex, 1000.0);
        chrono::Vector minBound;
        chrono::Vector maxBound; 
        addedObject->GetTotalAABB(minBound, maxBound);
        LargeObjectsBoundaries.push_back(std::tuple<chrono::Vector, chrono::Vector>(minBound, maxBound));
    }
}

void Scene::AddSmallObjects(chrono::ChSystemNSC& mphysicalSystem, ObjectLibrary library, std::default_random_engine& randomEngine, int maxNumberOfSmallObject, bool placeOnLargeObject)
{
    // TODO : check if useful to keep it here or keep a constant object
    // Generate the common material for the scene object
    auto mat = chrono_types::make_shared<chrono::ChMaterialSurfaceNSC>();
    mat->SetFriction(0.8f);
    mat->SetRollingFriction(0.9f);
    mat->SetSpinningFriction(0.9f);
    mat->SetCompliance(0.0);
    mat->SetComplianceT(0.0);
    mat->SetDampingF(1.0f);

    bool useConvex = false;
    
    double xmin = SceneBoundingBoxMin[0] + 0.3;
    double xmax = SceneBoundingBoxMax[0] - 0.3;
    double ymin = SceneBoundingBoxMin[1] + 0.3;
    double ymax = SceneBoundingBoxMax[1] - 0.3;
    // Just to let it fall a bit longer
    double zmin = SceneBoundingBoxMin[2] + 0.6;
    double zmax = SceneBoundingBoxMax[2] - 0.3;

    std::uniform_real_distribution<double> randomPositionX(xmin, xmax);
    std::uniform_real_distribution<double> randomPositionY(ymin, ymax);
    std::uniform_real_distribution<double> randomPositionZ(zmin, zmax);

    std::uniform_real_distribution<double> randomAngleGiver(-M_PI, M_PI);
    std::uniform_int_distribution<int> randomNumberOfSmallObject(std::min(5, maxNumberOfSmallObject), maxNumberOfSmallObject);
    int numberOfSmallObjects = randomNumberOfSmallObject(randomEngine);
    for (int i = 0; i < numberOfSmallObjects; i++)
    {
        if (placeOnLargeObject)
        {
            std::tuple<chrono::Vector, chrono::Vector> bound = LargeObjectsBoundaries[i % LargeObjectsBoundaries.size()];
            randomPositionX = std::uniform_real_distribution<double>(std::max(std::get<0>(bound)[0], xmin), std::min(std::get<1>(bound)[0], xmax));
            randomPositionY = std::uniform_real_distribution<double>(std::max(std::get<0>(bound)[1], ymin), std::min(std::get<1>(bound)[1], ymax));
            randomPositionZ = std::uniform_real_distribution<double>(std::max(std::get<0>(bound)[2], zmin), std::min(std::get<1>(bound)[2], zmax));
        }
        chrono::Vector position(randomPositionX(randomEngine), randomPositionY(randomEngine), randomPositionZ(randomEngine));
        AddObject(mphysicalSystem, library.GiveRandomObject(randomEngine, false), mat, randomEngine, position, randomAngleGiver(randomEngine), useConvex, 10.0);
    }
}


void Scene::AddGround(chrono::ChSystemNSC& mphysicalSystem)
{
    // Create the floor using
    // fixed rigid body of 'box' type:
    auto mat = chrono_types::make_shared<chrono::ChMaterialSurfaceNSC>();
    mat->SetFriction(0.9f);
    mat->SetRollingFriction(0.9f);
    mat->SetSpinningFriction(0.9f);
    mat->SetCompliance(0.0);
    mat->SetComplianceT(0.0);
    mat->SetDampingF(1.0f);

    auto mrigidFloor = chrono_types::make_shared <chrono::ChBodyEasyBox> (250, 4, 250,  // x,y,z size
        1000,         // density
        true,         // visualization?
        true,         // collision?
        mat);         // contact material
    mrigidFloor->SetPos(chrono::ChVector<>(0, -2, 0));
    mrigidFloor->SetBodyFixed(true);

    mphysicalSystem.Add(mrigidFloor);
}

std::shared_ptr<chrono::ChBody> Scene::AddObject(chrono::ChSystemNSC& mphysicalSystem, Object object, std::shared_ptr<chrono::ChMaterialSurfaceNSC>& commonMaterial,
    std::default_random_engine randomEngine, chrono::Vector position, double rotationAngle, bool useConvexHull, double mass, bool fixed, bool addToSystem)
{
    auto mesh = chrono_types::make_shared<chrono::geometry::ChTriangleMeshConnected>();

    mesh->LoadWavefrontMesh(object.AssociatedFile);


    double scale = fixed ? 1.0 : object.ComputeWantedScale(randomEngine, mesh);
    mesh->Transform(chrono::Vector(0.0, 0.0, 0.0), chrono::ChMatrix33<>(scale));
    std::cout << mesh->RepairDuplicateVertexes(1e-3) << std::endl;

    // We correct position corresponding to the object dimension
    // Note that if the object is too big, the correction will not work and the object will disappear at a "bad contact check".
    double xmin, xmax, ymin, ymax, zmin, zmax;
    object.ComputeMeshBounds(mesh, xmin, xmax, ymin, ymax, zmin, zmax);
    if (position[0] - xmin < SceneBoundingBoxMin[0])
        position[0] += xmin - position[0] + 0.001;
    else if (position[0] + xmax > SceneBoundingBoxMax[0])
        position[0] -= xmax - position[0] + 0.001;
    // Object will fall, no need to correct ymax with the "No scene layout in the engine at the beginning strategy".
    if (position[1] - ymin < SceneBoundingBoxMin[1])
        position[1] += ymin - position[1] + 0.001;
    if (position[2] - zmin < SceneBoundingBoxMin[2])
        position[2] += zmin - position[2] + 0.001;
    else if (position[2] + zmax > SceneBoundingBoxMax[2])
        position[2] -= zmax - position[2] + 0.001;

    if (useConvexHull)
    {     
        auto convexHull = chrono_types::make_shared< chrono::ChBodyEasyConvexHull>(mesh->getCoordsVertices(), mass, true, true, commonMaterial);
        convexHull->SetBodyFixed(fixed);
        convexHull->SetPos(position);
        if (rotationAngle != 0.0)
            convexHull->SetRot(chrono::ChMatrix33<>(
                chrono::Vector(std::cos(rotationAngle), 0.0, -std::sin(rotationAngle)),
                chrono::Vector(0.0, 1.0, 0.0),
                chrono::Vector(std::sin(rotationAngle), 0.0, std::cos(rotationAngle))));
        convexHull->SetShowCollisionMesh(true);

        if (addToSystem)
            mphysicalSystem.Add(convexHull);

        if (!fixed)
            MovingObjects.push_back(PlacedObject(object, scale, convexHull));


        return convexHull;

        // TODO only a convex hull doesn't seem to work
        //collisionObject->GetCollisionModel()->ClearModel();
        //std::vector<chrono::Vector> vertices = mesh->getCoordsVertices();
        //collisionObject->GetCollisionModel()->AddConvexHull(commonMaterial, vertices, chrono::VNULL, chrono::ChMatrix33<>(1));
    }
    else
    {
        auto collisionObject = chrono_types::make_shared<chrono::ChBody>();

        collisionObject->SetPos(position);
        if (rotationAngle != 0.0)
            collisionObject->SetRot(chrono::ChMatrix33<>(
                chrono::Vector(std::cos(rotationAngle), 0.0, -std::sin(rotationAngle)),
                chrono::Vector(0.0, 1.0, 0.0),
                chrono::Vector(std::sin(rotationAngle), 0.0, std::cos(rotationAngle))));
        collisionObject->SetMass(mass);
        collisionObject->SetBodyFixed(fixed);
        collisionObject->SetCollide(true);

        collisionObject->GetCollisionModel()->ClearModel();
        collisionObject->GetCollisionModel()->AddTriangleMesh(commonMaterial, mesh, false, false, chrono::VNULL, chrono::ChMatrix33<>(1), 0.005);
        collisionObject->GetCollisionModel()->BuildModel();

        auto mesh_asset = chrono_types::make_shared<chrono::ChTriangleMeshShape>();
        mesh_asset->SetMesh(mesh);
        mesh_asset->SetBackfaceCull(true);
        collisionObject->AddAsset(mesh_asset);
        collisionObject->SetUseSleeping(true);

        if (addToSystem)
            mphysicalSystem.Add(collisionObject);

        if (!fixed)
            MovingObjects.push_back(PlacedObject(object, scale, collisionObject));

        return collisionObject;
    }

}


double Scene::GetSceneArea()
{
    return (SceneBoundingBoxMax[0] - SceneBoundingBoxMin[0]) * (SceneBoundingBoxMax[1] - SceneBoundingBoxMin[1]);
}