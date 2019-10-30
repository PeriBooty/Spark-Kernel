#include <lib/spinlock.hpp>
#include <hardware/lock.hpp>

void spinlock_lock(Spinlock *lock) {
	cpu_atomic_loop_test_and_set(&lock->locked);
}

void spinlock_release(Spinlock *lock) {
	cpu_atomic_unset(&lock->locked);
}