/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include "Base.h"
#include "input/Input.h"
#include "renderer/api/Renderer.h"
#include "UI.h"

#include "RocketInterface.h"
#include "ImGuiInterface.h"

namespace dw {

#define INTERFACE_RENDER_QUEUE (Ogre::RENDER_QUEUE_OVERLAY)

const EventType EvtData_UIMouseEnter::eventType(0xe135de7);
const EventType EvtData_UIMouseLeave::eventType(0xe135de8);
const EventType EvtData_UIClick::eventType(0xe135dd7);
const EventType EvtData_UISubmit::eventType(0x3d02cddc);

UI::UI(Renderer* rs, Input* im, LuaState* ls) : mRenderSystem(rs) {
    // WORKAROUND: Deal with an ogre bug by call 'getQueueGroup' to ensure it actually exists
    rs->getSceneMgr()->getRenderQueue()->getQueueGroup(INTERFACE_RENDER_QUEUE);
    rs->getSceneMgr()->addRenderQueueListener(this);

    // Build projection matrix
    buildProjMatrix(mProjection);

    // Load UI material
    mUIMaterial = Ogre::MaterialManager::getSingleton().getByName("UI");
    if (!mUIMaterial->isLoaded())
        mUIMaterial->load();

    // Set up libRocket
    mRocketInterface = makeShared<RocketInterface>(rs, mUIMaterial, mProjection);
    Rocket::Core::SetRenderInterface(mRocketInterface.get());
    Rocket::Core::SetSystemInterface(mRocketInterface.get());
    Rocket::Core::SetFileInterface(mRocketInterface.get());
    Rocket::Core::Initialise();
    Rocket::Controls::Initialise();
    mContext = Rocket::Core::CreateContext("default",
                                           Rocket::Core::Vector2i(rs->getWidth(), rs->getHeight()));
    Rocket::Debugger::Initialise(mContext);

    // Load fonts
    Rocket::Core::FontDatabase::LoadFontFace("LiberationSans-Regular.ttf", "Liberation",
                                             Rocket::Core::Font::STYLE_NORMAL,
                                             Rocket::Core::Font::WEIGHT_NORMAL);

    // Initialise ImGui
    mImGuiInterface = makeShared<ImGuiInterface>(rs, im, mUIMaterial, mProjection);

    // Set up the console
    mConsole = makeShared<Console>(this, ls);

    // Event Delegates
    ADD_LISTENER(UI, EvtData_TextInput);
    ADD_LISTENER(UI, EvtData_KeyDown);
    ADD_LISTENER(UI, EvtData_KeyUp);
    ADD_LISTENER(UI, EvtData_MouseDown);
    ADD_LISTENER(UI, EvtData_MouseUp);
    ADD_LISTENER(UI, EvtData_MouseMove);
    ADD_LISTENER(UI, EvtData_MouseWheel);
}

UI::~UI() {
    REMOVE_LISTENER(UI, EvtData_TextInput);
    REMOVE_LISTENER(UI, EvtData_KeyDown);
    REMOVE_LISTENER(UI, EvtData_KeyUp);
    REMOVE_LISTENER(UI, EvtData_MouseDown);
    REMOVE_LISTENER(UI, EvtData_MouseUp);
    REMOVE_LISTENER(UI, EvtData_MouseMove);
    REMOVE_LISTENER(UI, EvtData_MouseWheel);

    mConsole.reset();

    // Shut down ImGUI
    mImGuiInterface.reset();

    // Shut down libRocket
    mContext->RemoveReference();
    Rocket::Core::Shutdown();
    mRocketInterface.reset();

    mRenderSystem->getSceneMgr()->removeRenderQueueListener(this);
}

void UI::beginFrame() {
    mImGuiInterface->beginFrame();
}

void UI::update(float dt) {
}

void UI::preRender() {
}

Layout* UI::loadLayout(const String& filename) {
    // TODO: Deal with failure correctly
    auto fn = Rocket::Core::String(filename.c_str());
    Rocket::Core::ElementDocument* document = mContext->LoadDocument(fn);

    // Set the title
    Rocket::Core::Element* title = document->GetElementById("title");
    if (title)
        title->SetInnerRML(document->GetTitle());

    return new Layout(this, document);
}

void UI::unloadLayout(Layout* layout) {
    delete layout;
}

void UI::handleEvent(EventDataPtr eventData) {
    if (eventIs<EvtData_TextInput>(eventData)) {
        auto castedEventData = castEvent<EvtData_TextInput>(eventData);

        mImGuiInterface->onTextInput(castedEventData->text);

        if (castedEventData->text != "`")
            mContext->ProcessTextInput(Rocket::Core::String(castedEventData->text.c_str()));
    }

    if (eventIs<EvtData_KeyDown>(eventData)) {
        auto castedEventData = castEvent<EvtData_KeyDown>(eventData);

        // Toggle the console
        if (castedEventData->keycode == SDLK_BACKQUOTE || castedEventData->keycode == SDLK_F12) {
            if (castedEventData->mod & KMOD_SHIFT) {
                Rocket::Debugger::SetVisible(!Rocket::Debugger::IsVisible());
            } else {
                mConsole->setVisible(!mConsole->isVisible());
            }
            return;
        }

        mImGuiInterface->onKey(castedEventData->keycode, castedEventData->mod, true);
        int key = mRocketInterface->mapSDLKeyCode(castedEventData->keycode);
        mContext->ProcessKeyDown(Rocket::Core::Input::KeyIdentifier(key),
                                 mRocketInterface->mapSDLKeyMod(castedEventData->mod));
    }

    if (eventIs<EvtData_KeyUp>(eventData)) {
        auto castedEventData = castEvent<EvtData_KeyUp>(eventData);
        mImGuiInterface->onKey(castedEventData->keycode, castedEventData->mod, false);
        int key = mRocketInterface->mapSDLKeyCode(castedEventData->keycode);
        mContext->ProcessKeyUp(Rocket::Core::Input::KeyIdentifier(key),
                               mRocketInterface->mapSDLKeyMod(castedEventData->mod));
    }

    if (eventIs<EvtData_MouseDown>(eventData)) {
        auto castedEventData = castEvent<EvtData_MouseDown>(eventData);
        mImGuiInterface->onMouseButton(castedEventData->button);
        mContext->ProcessMouseButtonDown(
            mRocketInterface->mapSDLMouseButton(castedEventData->button),
            mRocketInterface->mapSDLKeyMod(SDL_GetModState()));
    }

    if (eventIs<EvtData_MouseUp>(eventData)) {
        auto castedEventData = castEvent<EvtData_MouseUp>(eventData);
        mContext->ProcessMouseButtonUp(mRocketInterface->mapSDLMouseButton(castedEventData->button),
                                       mRocketInterface->mapSDLKeyMod(SDL_GetModState()));
    }

    if (eventIs<EvtData_MouseMove>(eventData)) {
        auto castedEventData = castEvent<EvtData_MouseMove>(eventData);
        mContext->ProcessMouseMove(castedEventData->pos.x, castedEventData->pos.y,
                                   mRocketInterface->mapSDLKeyMod(SDL_GetModState()));
    }

    if (eventIs<EvtData_MouseWheel>(eventData)) {
        auto castedEventData = castEvent<EvtData_MouseWheel>(eventData);
        mImGuiInterface->onMouseScroll(castedEventData->motion.y);
        mContext->ProcessMouseWheel(-castedEventData->motion.y,
                                    mRocketInterface->mapSDLKeyMod(SDL_GetModState()));
    }
}

void UI::ProcessEvent(Rocket::Core::Event& event) {
    String type = event.GetType().CString();
    String id = event.GetCurrentElement()->GetId().CString();

    Map<String, String> parameters;

    // Log event details
    std::stringstream out;
    out << "[librocket event] type: " << type << ", id: " << id << ", params: (";
    int i = 0;
    Rocket::Core::String key;
    Rocket::Core::String value;
    int totalLength = out.str().length();
    while (event.GetParameters()->Iterate(i, key, value)) {
        int thisLength = key.Length() + value.Length() + 2;
        if (totalLength + thisLength > 120) {
            out << std::endl << "[librocket event]   ";
            totalLength = thisLength + 20;
        } else {
            totalLength += thisLength;
        }

        // Print parameters
        out << key.CString() << "=" << value.CString() << ",";
        parameters.insert(makePair<String, String>(key.CString(), value.CString()));
    }
    out << ")";
    LOG << out.str();

    if (type == "mouseover")
        EventSystem::inst().queueEvent(makeShared<EvtData_UIMouseEnter>(id, parameters));
    if (type == "mouseout")
        EventSystem::inst().queueEvent(makeShared<EvtData_UIMouseLeave>(id, parameters));
    if (type == "click")
        EventSystem::inst().queueEvent(makeShared<EvtData_UIClick>(id, parameters));
    if (type == "submit")
        EventSystem::inst().queueEvent(makeShared<EvtData_UISubmit>(id, parameters));
}

void UI::renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String&, bool&) {
    if (queueGroupId == INTERFACE_RENDER_QUEUE &&
        Ogre::Root::getSingleton().getRenderSystem()->_getViewport()->getOverlaysEnabled()) {
        mContext->Update();
        mContext->Render();
        ImGui::Render();
    }
}

void UI::buildProjMatrix(Ogre::Matrix4& projectionMatrix) {
    float zNear = -1.0f;
    float zFar = 1.0f;

    projectionMatrix = Ogre::Matrix4::ZERO;
    projectionMatrix[0][0] = 2.0f / (float)mRenderSystem->getWidth();
    projectionMatrix[0][3] = -1.0f;
    projectionMatrix[1][1] = -2.0f / (float)mRenderSystem->getHeight();
    projectionMatrix[1][3] = 1.0f;
    projectionMatrix[2][2] = -2.0f / (zFar - zNear);
    projectionMatrix[3][3] = 1.0f;
}
}  // namespace dw
