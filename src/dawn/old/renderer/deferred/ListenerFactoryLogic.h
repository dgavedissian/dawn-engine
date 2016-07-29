/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

namespace dw {

// The simple types of compositor logics will all do the same thing - Attach a Listener to the
// created compositor
class DW_API ListenerFactoryLogic : public Ogre::CompositorLogic
{
public:
    virtual void compositorInstanceCreated(Ogre::CompositorInstance* newInstance)
    {
        Ogre::CompositorInstance::Listener* Listener = CreateListener(newInstance);
        newInstance->addListener(Listener);
        mListeners[newInstance] = Listener;
    }

    virtual void compositorInstanceDestroyed(Ogre::CompositorInstance* destroyedInstance)
    {
        delete mListeners[destroyedInstance];
        mListeners.erase(destroyedInstance);
    }

protected:
    // This is the method that implementers will need to create
    virtual Ogre::CompositorInstance::Listener* CreateListener(
        Ogre::CompositorInstance* instance) = 0;

private:
    typedef Map<Ogre::CompositorInstance*, Ogre::CompositorInstance::Listener*> ListenerMap;
    ListenerMap mListeners;
};

}
