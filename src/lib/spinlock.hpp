#pragma once

struct Spinlock {
	volatile int locked;
};

void spinlock_lock(Spinlock *lock);
void spinlock_release(Spinlock *lock);