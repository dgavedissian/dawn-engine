/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#if 0
#include "Common.h"
#include "Graphics/RenderSystem.h"
#include "Input/InputManager.h"
#include "UI/InterfaceManager.h"
#include "StateManager.h"
#include "SandboxState.h"

#include "Scene/space/Planet.h"
#include "Scene/space/Star.h"
#include "Scene/space/StarSystem.h"

#include "Engine.h"
#include "DefaultCameraManager.h"

NAMESPACE_BEGIN

SandboxState::SandboxState(Engine* engine)
    : mEngine(engine),
      mCamera(nullptr),
      mData(nullptr),
      mTime(static_cast<double>(std::time(NULL))),
      mDeltaTime(1.0),
      mTrackedObject(nullptr)
{
}

SandboxState::~SandboxState()
{
}

void SandboxState::HandleEvent(EventDataPtr eventData)
{
    if (EventIs<EvtData_KeyDown>(eventData))
    {
        auto castedEventData = static_pointer_cast<EvtData_KeyDown>(eventData);
        switch (castedEventData->keycode)
        {
        case SDLK_EQUALS:
            mDeltaTime *= 2.0;
            break;

        case SDLK_MINUS:
            mDeltaTime *= 0.5;
            break;

        default:
            break;
        }
    }

    if (EventIs<EvtData_MouseWheel>(eventData))
    {
        auto castedEventData = static_pointer_cast<EvtData_MouseWheel>(eventData);

        auto camera = mEngine->GetCameraMgr();
        float speed = camera->GetMovementSpeed() * (float)std::pow(1.5f, castedEventData->motion.y);
        camera->SetMovementSpeed(speed);
        UpdateUI(speed);
    }
}

void SandboxState::Enter()
{
    auto camera = mEngine->GetMainCamera();
    camera->SetControlState(CCS_FREE);
    camera->SetDragEventState(CDES_RMB_DOWN);
    camera->ToggleParticles(false);
    camera->SetMovementSpeed(1000.0f);

    mData = mEngine->GetInterfaceMgr()->LoadLayout("universe.rml");
    {
        shared_ptr<Star> star;
        shared_ptr<Planet> planet, moon;

        // Reset anything that was already there
        // TODO: HACK: Replace StarSystem singleton with a SceneManager that managers star
        // systems properly
        //StarSystem::release();
        StarSystem* system = mEngine->GetStarSystem();

        // Create the star at the centre of the system
        StarDesc s;
        s.radius = ConvUnit(695500.0f, UNIT_KM);
        s.spectralClass = SC_G;
        star = system->CreateStar(s);
        system->SetRoot(star);

        // Create the planet
        PlanetDesc p;
        p.radius = ConvUnit(6371.0f, UNIT_KM);
        p.rotationPeriod = 1.0f;
        p.axialTilt = math::pi / 32.0f;
        p.surfaceTexture = "planet.jpg";
        p.hasAtmosphere = true;
        p.atmosphere.radius = p.radius * 1.025f;
        planet = system->CreatePlanet(p);
        star->AddSatellite(planet, make_shared<CircularOrbit>(
                                       ConvUnit(1.0f, UNIT_AU), 400.0f));

        // Create a moon orbiting this planet
        PlanetDesc m;
        m.radius = ConvUnit(1200.0f, UNIT_KM);
        m.rotationPeriod = 30.0f;
        m.surfaceTexture = "moon.jpg";
        moon = system->CreatePlanet(m);
        planet->AddSatellite(moon, make_shared<CircularOrbit>(
                                       ConvUnit(22000.0f, UNIT_KM), 30.0f));

        mEngine->GetStarSystem()->CalculatePosition(0.0f);
        camera->SetPosition(planet->GetPosition() +
                            Position(ConvUnit(2000.0f, UNIT_KM), 0.0f,
                                     ConvUnit(8000.0f, UNIT_KM)));
    }

    ADD_LISTENER(SandboxState, EvtData_KeyDown);
    ADD_LISTENER(SandboxState, EvtData_MouseWheel);
}

void SandboxState::Exit()
{
    mEngine->GetInterfaceMgr()->UnloadLayout(mData);
    REMOVE_LISTENER(SandboxState, EvtData_KeyDown);
    REMOVE_LISTENER(SandboxState, EvtData_MouseWheel);
}

void SandboxState::Update(float dt)
{
    // Track the nearest system object
    SystemBody* nearest;
    float minDistance = std::numeric_limits<float>::infinity();
    std::function<void(SystemBody*)> findNearest =
        [&nearest, &minDistance, this, &findNearest](SystemBody* body)
    {
        float distance = body->GetPosition()
                             .GetRelativeToPoint(this->mEngine->GetMainCamera()->GetPosition())
                             .LengthSq();
        if (distance < minDistance)
        {
            minDistance = distance;
            nearest = body;
        }

        // Visit children
        for (auto c : body->GetAllSatellites())
            findNearest(c.get());
    };

    findNearest(mEngine->GetStarSystem()->GetRootBody().get());
    mTrackedObject = nearest;

    // Calculate delta pos from tracked object
    Position delta = mEngine->GetCameraMgr()->GetPosition() - mTrackedObject->GetPosition();

    // Update the time
    mTime += mDeltaTime * dt;
    mEngine->GetStarSystem()->CalculatePosition(mTime / (24.0 * 3600.0));

    // Update camera position
    mEngine->GetCameraMgr()->SetPosition(mTrackedObject->GetPosition() + delta);

    // Update UI
    UpdateUI(mEngine->GetCameraMgr()->GetMovementSpeed());
}

void SandboxState::UpdateUI(float speed)
{
    // Format speed
    std::stringstream speedStr;
    speedStr << std::fixed << std::setprecision(2);
    if (speed > 10000000.0f)    // 10,000km/sec
    {
        // Represent in terms of speed of light
        speedStr << (speed / 299792458.0f) << "c";
    }
    else if (speed < 1000.0f)
    {
        // Represent in meters/sec
        speedStr << speed << "m/sec";
    }
    else
    {
        // Represent in kilometers/sec
        speedStr << (speed / 1000.0f) << "km/sec";
    }

    // Format time
    std::stringstream timeStr;
    time_t currentTime = static_cast<time_t>(mTime);
    timeStr << time::Format(currentTime, "%c") << " (+" << mDeltaTime << "s)";

    // Update UI
    mData->GetElementById("speed")->SetInnerRML(speedStr.str().c_str());
    mData->GetElementById("time")->SetInnerRML(timeStr.str().c_str());
}

NAMESPACE_END
#endif
