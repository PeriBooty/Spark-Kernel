#pragma once

namespace Spark {
    class Spinlock {
        volatile int locked;

    public:
        void lock();
        void release();
        ~Spinlock();
    };
}  // namespace Spark