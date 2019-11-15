#include <hardware/cpu/lock.hpp>
#include <lib/spinlock.hpp>

/// Locks a spinlock
void Spinlock::lock() {
    cpu_atomic_loop_test_and_set(&locked);
}

/// Releases a spinlock
void Spinlock::release() {
    cpu_atomic_unset(&locked);
}