#pragma once

/// Spinlock struct
class Spinlock {
private:
    volatile int locked;

public:
    void lock();
    void release();
};