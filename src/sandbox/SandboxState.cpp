/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "DawnEngine.h"
#include "SandboxState.h"

SandboxState::SandboxState(dw::Engine* engine)
    : mEngine(engine),
      mCamera(nullptr),
      mData(nullptr),
      mTime(static_cast<double>(std::time(NULL))),
      mDeltaTime(1.0),
      mTrackedObject(nullptr) {
    mCamera =
        new dw::DefaultCamera(engine->getRenderer(), engine->getInput(), engine->getSceneMgr());
    engine->setMainCamera(mCamera);

    // Create the star system
    // TODO: no
    engine->setStarSystem(new dw::StarSystem(engine->getRenderer(), engine->getPhysicsWorld()));
}

SandboxState::~SandboxState() {
    SAFE_DELETE(mCamera);
    delete mEngine->getStarSystem();
    mEngine->setStarSystem(nullptr);
}

void SandboxState::handleEvent(dw::EventDataPtr eventData) {
    if (dw::eventIs<dw::EvtData_KeyDown>(eventData)) {
        auto castedEventData = dw::castEvent<dw::EvtData_KeyDown>(eventData);
        switch (castedEventData->keycode) {
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

    if (dw::eventIs<dw::EvtData_MouseWheel>(eventData)) {
        auto castedEventData = dw::castEvent<dw::EvtData_MouseWheel>(eventData);

        auto camera = dynamic_cast<dw::DefaultCamera*>(mEngine->getMainCamera());
        float speed = camera->getMovementSpeed() * (float)std::pow(1.5f, castedEventData->motion.y);
        camera->setMovementSpeed(speed);
        UpdateUI(speed);
    }
}

void SandboxState::enter() {
    mCamera->setControlState(dw::CCS_FREE);
    mCamera->setDragEventState(dw::CDES_RMB_DOWN);
    mCamera->toggleParticles(false);
    mCamera->setMovementSpeed(10000.0f);

    mData = mEngine->getUI()->loadLayout("universe.rml");

    // TODO: script this
    {
        dw::SharedPtr<dw::Star> star;
        dw::SharedPtr<dw::Planet> planet, moon;

        // Reset anything that was already there
        // TODO: HACK: Replace StarSystem singleton with a SceneManager that managers star
        // systems properly
        // StarSystem::release();
        dw::StarSystem* system = mEngine->getStarSystem();

        // Create the star at the centre of the system
        dw::StarDesc s;
        s.radius = dw::convUnit(695500.0f, dw::UNIT_KM);
        s.spectralClass = dw::SC_G;
        star = system->createStar(s);
        system->setRoot(star);

        // Create the planet
        dw::PlanetDesc p;
        p.radius = dw::convUnit(6371.0f, dw::UNIT_KM);
        p.rotationPeriod = 1.0f;
        p.axialTilt = math::pi / 32.0f;
        p.surfaceTexture = "planet.jpg";
        p.hasAtmosphere = true;
        p.atmosphere.radius = p.radius * 1.025f;
        p.hasRings = true;
        p.rings.minRadius = p.radius * 1.25f;
        p.rings.maxRadius = p.radius * 2.65f;
        p.rings.texture = "rings.png";
        planet = system->createPlanet(p);
        star->addSatellite(
            planet, dw::makeShared<dw::CircularOrbit>(dw::convUnit(1.0f, dw::UNIT_AU), 400.0f));

        // Create a moon orbiting this planet
        dw::PlanetDesc m;
        m.radius = dw::convUnit(1200.0f, dw::UNIT_KM);
        m.rotationPeriod = 30.0f;
        m.surfaceTexture = "moon.jpg";
        moon = system->createPlanet(m);
        planet->addSatellite(
            moon, dw::makeShared<dw::CircularOrbit>(dw::convUnit(22000.0f, dw::UNIT_KM), 30.0f));

        mEngine->getStarSystem()->calculatePosition(0.0f);
        mCamera->setPosition(planet->getPosition() +
                             dw::Position(dw::convUnit(2000.0f, dw::UNIT_KM), 0.0f,
                                          dw::convUnit(8000.0f, dw::UNIT_KM)));
    }

    ADD_LISTENER(SandboxState, dw::EvtData_KeyDown);
    ADD_LISTENER(SandboxState, dw::EvtData_MouseWheel);
}

void SandboxState::exit() {
    mEngine->getUI()->unloadLayout(mData);
    REMOVE_LISTENER(SandboxState, dw::EvtData_KeyDown);
    REMOVE_LISTENER(SandboxState, dw::EvtData_MouseWheel);
}

void SandboxState::update(float dt) {
    mCamera->update(dt);

    // Track the nearest system object
    dw::SystemBody* nearest;
    float minDistance = std::numeric_limits<float>::infinity();
    std::function<void(dw::SystemBody*)> findNearest = [&nearest, &minDistance, this,
                                                        &findNearest](dw::SystemBody* body) {
        float distance = body->getPosition()
                             .getRelativeTo(this->mEngine->getMainCamera()->getPosition())
                             .LengthSq();
        if (distance < minDistance) {
            minDistance = distance;
            nearest = body;
        }

        // Visit children
        for (auto c : body->getAllSatellites())
            findNearest(c.get());
    };

    findNearest(mEngine->getStarSystem()->getRootBody().get());
    mTrackedObject = nearest;

    // Calculate delta pos from tracked object
    dw::Position delta = mCamera->getPosition() - mTrackedObject->getPosition();

    // Update the time
    mTime += mDeltaTime * dt;
    mEngine->getStarSystem()->calculatePosition(mTime / (24.0 * 3600.0));

    // Update camera position
    mCamera->setPosition(mTrackedObject->getPosition() + delta);

    // Update UI
    UpdateUI(mCamera->getMovementSpeed());
}

void SandboxState::UpdateUI(float speed) {
    // Format speed
    std::stringstream speedStr;
    speedStr << std::fixed << std::setprecision(2);
    if (speed > 10000000.0f)  // 10,000km/sec
    {
        // Represent in terms of speed of light
        speedStr << (speed / 299792458.0f) << "c";
    } else if (speed < 1000.0f) {
        // Represent in meters/sec
        speedStr << speed << "m/sec";
    } else {
        // Represent in kilometers/sec
        speedStr << (speed / 1000.0f) << "km/sec";
    }

    // Format time
    std::stringstream timeStr;
    time_t currentTime = static_cast<time_t>(mTime);
    timeStr << dw::time::format(currentTime, "%c") << " (+" << mDeltaTime << "s)";

    // Update UI
    mData->getElementById("speed")->SetInnerRML(speedStr.str().c_str());
    mData->getElementById("time")->SetInnerRML(timeStr.str().c_str());
}
