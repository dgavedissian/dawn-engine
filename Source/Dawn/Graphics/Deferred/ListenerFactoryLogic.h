/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

// The simple types of compositor logics will all do the same thing - Attach a listener to the
// created compositor
class DW_API ListenerFactoryLogic : public Ogre::CompositorLogic
{
public:
    virtual void compositorInstanceCreated(Ogre::CompositorInstance* newInstance)
    {
        Ogre::CompositorInstance::Listener* listener = CreateListener(newInstance);
        newInstance->addListener(listener);
        mListeners[newInstance] = listener;
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
    typedef std::map<Ogre::CompositorInstance*, Ogre::CompositorInstance::Listener*> ListenerMap;
    ListenerMap mListeners;
};

NAMESPACE_END
