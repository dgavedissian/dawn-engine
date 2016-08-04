/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

namespace dw {

class Thread {
public:
    Thread();
    ~Thread();

    void join();

private:
    std::thread mThread;
};
}
