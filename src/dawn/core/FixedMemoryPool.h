/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {

class MemoryPool {
public:
    MemoryPool() {
    }
    virtual ~MemoryPool() {
    }

    template <class T, class... Args> T* alloc(Args... args) {
        return new (internalAlloc()) T(args...);
    }

    template <class T> void free(T* object) {
        object->~T();
        internalFree(object);
    }

protected:
    virtual void* internalAlloc() = 0;
    virtual void internalFree(void* object) = 0;
};

template <class T> class FixedMemoryPool : public MemoryPool {
public:
    FixedMemoryPool(uint slots) {
        allocateBlock(slots);
    }
    virtual ~FixedMemoryPool() {
        freeBlock();
    }

private:
    void* internalAlloc() override {
        T* object = mFreeList.front();
        mFreeList.pop_front();
        return reinterpret_cast<void*>(object);
    }

    void internalFree(void* object) override {
        mFreeList.push_back(reinterpret_cast<T*>(object));
    }

    void allocateBlock(uint slots) {
        mBlock = reinterpret_cast<T*>(operator new(slots * sizeof(T)));
        for (uint i = 0; i < slots; i++)
            mFreeList.push_back(mBlock + i);
    }

    void freeBlock() {
        operator delete(mBlock);
        mFreeList.clear();
    }

    T* mBlock;
    List<T*> mFreeList;
};
}
