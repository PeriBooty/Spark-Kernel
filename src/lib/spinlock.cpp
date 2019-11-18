#include <hardware/cpu/cpu.hpp>
#include <lib/spinlock.hpp>

/// Locks a spinlock
void Spark::Spinlock::lock() {
    Cpu::atomic_set(&locked);
}

/// Releases a spinlock
void Spark::Spinlock::release() {
    Cpu::atomic_unset(&locked);
}

Spark::Spinlock::~Spinlock() {
    release();
}