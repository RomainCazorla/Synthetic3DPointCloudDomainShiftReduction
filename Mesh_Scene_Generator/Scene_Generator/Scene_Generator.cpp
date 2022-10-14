// =============================================================================
// PROJECT CHRONO available at : http://projectchrono.org
//
// Author : Romain Cazorla
// =============================================================================


#include "chrono/physics/ChSystemNSC.h"
#include "chrono/solver/ChSolverPSOR.h"
#include "chrono/assets/ChTexture.h"
#include "chrono/physics/ChBodyEasy.h"

#include "chrono_irrlicht/ChIrrApp.h"

#include "ObjectLibrary.h"
#include "Scene.h"
#include "CheckCollisions.h"

// Use the namespaces of Chrono
using namespace chrono;
using namespace chrono::irrlicht;

// Use the main namespaces of Irrlicht
using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;
using namespace irr::io;
using namespace irr::gui;


Scene create_scene(ChSystemNSC& mphysicalSystem, ObjectLibrary scenesLibrary, std::default_random_engine& randomEngine)
{
    Scene scene;
    scene.AddLayout(mphysicalSystem, scenesLibrary, randomEngine, false);
    scene.AddGround(mphysicalSystem);

    return scene;
}

void OutputSimulationToStream(std::ostream& outputStream, chrono::ChSystem& mphysicalSystem, std::string& layoutPath, std::vector<PlacedObject>& objects)
{
    auto collision_checker = chrono_types::make_shared<CheckCollisions>();
    mphysicalSystem.GetContactContainer()->ReportAllContacts(collision_checker);

    outputStream << "layout_file:" << layoutPath << std::endl;
    for (auto object : objects)
    {
        PlacedObject& this_object = object;
        std::shared_ptr<ChBody>body = this_object.CollisionBody;
        ChContactable* a_contact = body->GetCollisionModel()->GetContactable();
        if (collision_checker->contacts[a_contact])
        {
            std::cout << "Body in bad contact" << this_object.BaseObject.AssociatedFile << std::endl;
            continue;
        }
        ChMatrix33<> myRot = body->Amatrix;
        ChVector<>   mypos = body->GetPos();
        if (mypos[1] < -7)
        {
            std::cout << "Body below threshold" << this_object.BaseObject.AssociatedFile << std::endl;
            continue;
        }
        ChVector<> speed = body->GetPos_dt();
        if (speed.Length() > 1.0)
        {
            std::cout << "Body still moving" << this_object.BaseObject.AssociatedFile << std::endl;
            std::cout << "Speed" << speed.Length() << std::endl;
            continue;
        }
        ChVector<> rot = body->GetWvel_loc();
        if (rot.Length() > 2.0)
        {
            std::cout << "Body still spinning" << this_object.BaseObject.AssociatedFile << std::endl;
            std::cout << "Rot" << rot.Length() << std::endl;
            continue;
        }
        outputStream << this_object.BaseObject.AssociatedFile << std::endl;
        outputStream << this_object.BaseObject.Wnids << std::endl;
        outputStream << this_object.Scale << std::endl;
        outputStream << myRot(0) << " " << myRot(1) << " " << myRot(2) << " " << mypos[0] << std::endl;
        outputStream << myRot(3) << " " << myRot(4) << " " << myRot(5) << " " << mypos[1] << std::endl;
        outputStream << myRot(6) << " " << myRot(7) << " " << myRot(8) << " " << mypos[2] << std::endl;

        std::cout << "Adding " << this_object.BaseObject.AssociatedFile << " at x:" << mypos[0] << " y:" << mypos[1] << " z:" << mypos[2] << std::endl;
    }
}

void removeBadContactObjects(chrono::ChSystem& mphysicalSystem, Scene& scene)
{
    auto collision_checker = chrono_types::make_shared<CheckCollisions>();
    mphysicalSystem.GetContactContainer()->ReportAllContacts(collision_checker);

    std::vector<chrono::ChContactable*> toIgnore;

    for (int i = scene.MovingObjects.size() - 1; i > -1; i--)
    {
        std::shared_ptr<chrono::ChBody> currentObject = scene.MovingObjects[i].CollisionBody;
        ChContactable* contact = currentObject->GetCollisionModel()->GetContactable();
        if (collision_checker->contacts[contact])
        {
            if (!(std::find(toIgnore.begin(), toIgnore.end(), contact) != toIgnore.end()))
            {
                toIgnore.push_back(collision_checker->linkedContactable[contact]);
                std::cout << "Removing from scene due to bad contact " << scene.MovingObjects[i].BaseObject.AssociatedFile << std::endl;
                mphysicalSystem.RemoveBody(currentObject);
                scene.MovingObjects.erase(scene.MovingObjects.begin() + i);
            }
            else
            {
                std::cout << "Uping due to bad contact " << scene.MovingObjects[i].BaseObject.AssociatedFile << std::endl;
                currentObject->RemoveAllForces();
                chrono::Vector currentPos = currentObject->GetPos();
                currentObject->SetPos(chrono::Vector(currentPos[0], currentPos[1] + 0.1, currentPos[2]));
            }
        }
    }
}

void runApplicationFor(ChIrrApp& application, int numberOfStep)
{
    for (int i = 0; i < numberOfStep; i++)
    {
        application.BeginScene(true, true, SColor(255, 140, 161, 192));

        tools::drawGrid(application.GetVideoDriver(), 5, 5, 20, 20,
            ChCoordsys<>(ChVector<>(0, 0.04, 0), Q_from_AngAxis(CH_C_PI / 2, VECT_X)),
            video::SColor(50, 90, 90, 150), true);

        application.DrawAll();

        application.DoStep();

        application.EndScene();
    }
}

int main(int argc, char* argv[]) 
{
    // Create a ChronoENGINE physical system
    ChSystemNSC mphysicalSystem;
    std::random_device rd;
    std::mt19937 randomEngine(rd());

    ObjectLibrary scenesLibrary("PathTO\\SceneNetRGBD_Layouts-master\\office\\", "PathTO\\scene_description.txt",
        "PathTO\\scene_probability.txt");
    Scene scene = create_scene(mphysicalSystem, scenesLibrary, randomEngine);

    ObjectLibrary library("PathTO\\ShapeNet\\", "PathTO\\library_description.txt",
        "PathTO\\large_object_probability.txt",
        "PathTO\\small_object_probability.txt");
    scene.AddLargeObjects(mphysicalSystem, library, randomEngine, (int)scene.GetSceneArea() / 2.0);


    auto solver = chrono_types::make_shared<ChSolverPSOR>();
    solver->SetMaxIterations(40);
    solver->EnableWarmStart(true);
    mphysicalSystem.SetSolver(solver);

    mphysicalSystem.SetMaxPenetrationRecoverySpeed(2.0);
    mphysicalSystem.SetMinBounceSpeed(50.0);

    bool visualisation = true;
    // Simulation loop

    if (visualisation)
    {
        // Create the Irrlicht visualization
        ChIrrApp application(&mphysicalSystem, L"Scene creation", core::dimension2d<u32>(800, 600));
        application.AddTypicalLogo();
        application.AddTypicalSky();
        application.AddTypicalLights(core::vector3df(70.f, 120.f, -90.f), core::vector3df(30.f, 80.f, 60.f), 290, 190);
        application.AddTypicalCamera(core::vector3df(12, 3.5, -5), core::vector3df(0, 3, 0));
        // Add a ChIrrNodeAsset to all items
        application.AssetBindAll();

        // Convert meshes into visible meshes
        application.AssetUpdateAll();
        double timeStep = 0.02;
        application.SetTimestep(timeStep);
        application.SetTryRealtime(true);

        while (application.GetDevice()->run())
        {
            runApplicationFor(application, 0.5 / timeStep);
            //system("pause");
            removeBadContactObjects(mphysicalSystem, scene);
            application.AssetBindAll();
            application.AssetUpdateAll();

            for (int i = 0; i < 9; i++)
            {
                runApplicationFor(application, 0.5 / timeStep);
                removeBadContactObjects(mphysicalSystem, scene);
                application.AssetBindAll();
                application.AssetUpdateAll();
            }

            std::cout << "Adding small objects" << std::endl;
            scene.AddSmallObjects(mphysicalSystem, library, randomEngine, (int)scene.GetSceneArea() * 2, true);
            application.AssetBindAll();
            application.AssetUpdateAll();
            runApplicationFor(application, 0.04 / timeStep);
            //system("pause");
            removeBadContactObjects(mphysicalSystem, scene);
            application.AssetBindAll();
            application.AssetUpdateAll();

            for (int i = 0; i < 9; i++)
            {
                runApplicationFor(application, 0.5 / timeStep);
                removeBadContactObjects(mphysicalSystem, scene);
                application.AssetBindAll();
                application.AssetUpdateAll();
            }

            std::cout << "Adding scene" << std::endl;
            mphysicalSystem.AddBody(scene.SceneLayout);
            application.AssetBindAll();
            application.AssetUpdateAll();

            runApplicationFor(application, 1);
            removeBadContactObjects(mphysicalSystem, scene);
            application.AssetBindAll();
            application.AssetUpdateAll();

            runApplicationFor(application, 0.25 / timeStep);

            break;
        }
    }
    else
    {
        std::cout << "Beginning simulation" << std::endl;
        mphysicalSystem.SetStep(0.02);

        mphysicalSystem.DoEntireDynamics(0.04);
        removeBadContactObjects(mphysicalSystem, scene);
        mphysicalSystem.DoEntireDynamics(5.0);

        std::cout << "Mid simulation processing" << std::endl;
        mphysicalSystem.AddBody(scene.SceneLayout);

        removeBadContactObjects(mphysicalSystem, scene);
        mphysicalSystem.DoEntireDynamics(0.02);
        removeBadContactObjects(mphysicalSystem, scene);
        mphysicalSystem.DoEntireDynamics(0.25);

        std::cout << "Simulation ended" << std::endl;
    }

    std::ofstream outputStream("OutputPath");
    OutputSimulationToStream(outputStream, mphysicalSystem, scene.UsedLayout.AssociatedFile, scene.MovingObjects);
    outputStream.close();

    return 0;
}