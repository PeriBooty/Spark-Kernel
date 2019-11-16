#include <hardware/cpu/cpu.hpp>
#include <lib/spinlock.hpp>

/// Locks a spinlock
void Spinlock::lock() {
    CPU::atomic_loop_test_and_set(&locked);
}

/// Releases a spinlock
void Spinlock::release() {
    CPU::atomic_unset(&locked);
}