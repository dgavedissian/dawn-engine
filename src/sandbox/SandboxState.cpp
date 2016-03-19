/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "DawnEngine.h"
#include "SandboxState.h"

SandboxState::SandboxState(dw::Engine* engine)
    : mEngine(engine),
      mCamera(nullptr),
      mData(nullptr),
      mTime(static_cast<double>(std::time(NULL))),
      mDeltaTime(1.0),
      mTrackedObject(nullptr)
{
	mCamera = new dw::DefaultCamera(engine->GetRenderer(), engine->GetInput(), engine->GetSceneMgr());
	engine->SetMainCamera(mCamera);

	// Create the star system
	// TODO: no
    engine->SetStarSystem(new dw::StarSystem(engine->GetRenderer(), engine->GetPhysicsWorld()));
}

SandboxState::~SandboxState()
{
	SAFE_DELETE(mCamera);
	delete mEngine->GetStarSystem();
	mEngine->SetStarSystem(nullptr);
}

void SandboxState::HandleEvent(dw::EventDataPtr eventData)
{
    if (dw::EventIs<dw::EvtData_KeyDown>(eventData))
    {
        auto castedEventData = dw::CastEvent<dw::EvtData_KeyDown>(eventData);
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

    if (dw::EventIs<dw::EvtData_MouseWheel>(eventData))
    {
        auto castedEventData = dw::CastEvent<dw::EvtData_MouseWheel>(eventData);

        auto camera = dynamic_cast<dw::DefaultCamera*>(mEngine->GetMainCamera());
        float speed = camera->GetMovementSpeed() * (float)std::pow(1.5f, castedEventData->motion.y);
        camera->SetMovementSpeed(speed);
        UpdateUI(speed);
    }
}

void SandboxState::Enter()
{
	mCamera->SetControlState(dw::CCS_FREE);
	mCamera->SetDragEventState(dw::CDES_RMB_DOWN);
	mCamera->ToggleParticles(false);
	mCamera->SetMovementSpeed(10000.0f);

    mData = mEngine->GetUI()->LoadLayout("universe.rml");

	// TODO: script this
    {
		dw::SharedPtr<dw::Star> star;
        dw::SharedPtr<dw::Planet> planet, moon;

        // Reset anything that was already there
        // TODO: HACK: Replace StarSystem singleton with a SceneManager that managers star
        // systems properly
        //StarSystem::release();
		dw::StarSystem* system = mEngine->GetStarSystem();

        // Create the star at the centre of the system
		dw::StarDesc s;
        s.radius = dw::ConvUnit(695500.0f, dw::UNIT_KM);
        s.spectralClass = dw::SC_G;
        star = system->CreateStar(s);
        system->SetRoot(star);

        // Create the planet
		dw::PlanetDesc p;
        p.radius = dw::ConvUnit(6371.0f, dw::UNIT_KM);
        p.rotationPeriod = 1.0f;
        p.axialTilt = math::pi / 32.0f;
        p.surfaceTexture = "planet.jpg";
        p.hasAtmosphere = true;
        p.atmosphere.radius = p.radius * 1.025f;
        planet = system->CreatePlanet(p);
        star->AddSatellite(planet, dw::MakeShared<dw::CircularOrbit>(
			dw::ConvUnit(1.0f, dw::UNIT_AU), 400.0f));

        // Create a moon orbiting this planet
		dw::PlanetDesc m;
        m.radius = dw::ConvUnit(1200.0f, dw::UNIT_KM);
        m.rotationPeriod = 30.0f;
        m.surfaceTexture = "moon.jpg";
        moon = system->CreatePlanet(m);
        planet->AddSatellite(moon, dw::MakeShared<dw::CircularOrbit>(
			dw::ConvUnit(22000.0f, dw::UNIT_KM), 30.0f));

        mEngine->GetStarSystem()->CalculatePosition(0.0f);
		mCamera->SetPosition(planet->GetPosition() +
			dw::Position(dw::ConvUnit(2000.0f, dw::UNIT_KM), 0.0f,
				dw::ConvUnit(8000.0f, dw::UNIT_KM)));
    }

    ADD_LISTENER(SandboxState, dw::EvtData_KeyDown);
    ADD_LISTENER(SandboxState, dw::EvtData_MouseWheel);
}

void SandboxState::Exit()
{
    mEngine->GetUI()->UnloadLayout(mData);
    REMOVE_LISTENER(SandboxState, dw::EvtData_KeyDown);
    REMOVE_LISTENER(SandboxState, dw::EvtData_MouseWheel);
}

void SandboxState::Update(float dt)
{
	mCamera->Update(dt);

    // Track the nearest system object
	dw::SystemBody* nearest;
    float minDistance = std::numeric_limits<float>::infinity();
    std::function<void(dw::SystemBody*)> findNearest =
        [&nearest, &minDistance, this, &findNearest](dw::SystemBody* body)
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
	dw::Position delta = mCamera->GetPosition() - mTrackedObject->GetPosition();

    // Update the time
    mTime += mDeltaTime * dt;
    mEngine->GetStarSystem()->CalculatePosition(mTime / (24.0 * 3600.0));

    // Update camera position
	mCamera->SetPosition(mTrackedObject->GetPosition() + delta);

    // Update UI
    UpdateUI(mCamera->GetMovementSpeed());
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
    timeStr << dw::time::Format(currentTime, "%c") << " (+" << mDeltaTime << "s)";

    // Update UI
    mData->GetElementById("speed")->SetInnerRML(speedStr.str().c_str());
    mData->GetElementById("time")->SetInnerRML(timeStr.str().c_str());
}
