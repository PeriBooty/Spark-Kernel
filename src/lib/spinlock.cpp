#include <hardware/cpu/cpu.hpp>
#include <lib/spinlock.hpp>

/// Locks a spinlock
void Spinlock::lock() {
    Cpu::atomic_set(&locked);
}

/// Releases a spinlock
void Spinlock::release() {
    Cpu::atomic_unset(&locked);
}