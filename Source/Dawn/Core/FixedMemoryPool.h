/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#pragma once

NAMESPACE_BEGIN

class MemoryPool
{
public:
    MemoryPool() {}
    virtual ~MemoryPool() {}

    template <class T, class... Args>
    T* New(Args... args)
    {
        return new (Alloc()) T(args...);
    }

    template <class T>
    void Delete(T* object)
    {
        object->~T();
        Free(object);
    }

protected:
    virtual void* Alloc() = 0;
    virtual void Free(void* object) = 0;

};

template <class T>
class FixedMemoryPool : public MemoryPool
{
public:
    FixedMemoryPool(uint slots) { AllocateBlock(slots); }
    virtual ~FixedMemoryPool() { DeallocateBlock(); }

private:
    void* Alloc() override
    {
        T* object = mFreeList.front();
        mFreeList.pop_front();
        return reinterpret_cast<void*>(object);
    }

    void Free(void* object) override
    {
        mFreeList.push_back(reinterpret_cast<T*>(object));
    }

    void AllocateBlock(uint slots)
    {
        mBlock = reinterpret_cast<T*>(operator new(slots * sizeof(T)));
        for (uint i = 0; i < slots; i++)
            mFreeList.push_back(mBlock + i);
    }

    void DeallocateBlock()
    {
        delete mBlock;
        mFreeList.clear();
    }

    T* mBlock;
    std::list<T*> mFreeList;
};

NAMESPACE_END
