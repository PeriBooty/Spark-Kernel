#pragma once

/// Locks and waits for release of int var pointer
void cpu_atomic_loop_test_and_set(volatile int *var);

/// Releases int var pointer
void cpu_atomic_unset(volatile int *var);