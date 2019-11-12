#pragma once

/// Spinlock struct
struct Spinlock {
    volatile int locked;
};

/// Locks a spinlock
void spinlock_lock(Spinlock *lock);

/// Releases a spinlock
void spinlock_release(Spinlock *lock);