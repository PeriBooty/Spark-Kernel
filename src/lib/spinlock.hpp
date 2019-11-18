#pragma once

namespace Spark {
    /// Spinlock struct
    class Spinlock {
    private:
        volatile int locked;

    public:
        void lock();
        void release();
        ~Spinlock();
    };
}  // namespace Spark