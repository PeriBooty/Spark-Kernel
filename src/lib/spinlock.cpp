#include <hardware/cpu/lock.hpp>
#include <lib/spinlock.hpp>

/// Locks a spinlock
void spinlock_lock(Spinlock *lock) {
    cpu_atomic_loop_test_and_set(&lock->locked);
}

/// Releases a spinlock
void spinlock_release(Spinlock *lock) {
    cpu_atomic_unset(&lock->locked);
}