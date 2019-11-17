#include <hardware/cpu/cpu.hpp>
#include <lib/spinlock.hpp>

/// Locks a spinlock
void Spinlock::lock() {
    CentralProcessingUnit::atomic_set(&locked);
}

/// Releases a spinlock
void Spinlock::release() {
    CentralProcessingUnit::atomic_unset(&locked);
}