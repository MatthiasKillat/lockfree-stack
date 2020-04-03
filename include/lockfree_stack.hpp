// Copyright (c) 2020 by Matthias Killat. All rights reserved.
#pragma once

#include "index_stack.hpp"
#include <optional>

//TODO: experimental, tests and proof of algorithmic correctness are missing
//since it was just an afternoons thought there surely will be correctness issues:-)

/// @brief implements a lock free stack (i.e. container with LIFO order) of elements of type T
/// with Capacity
template <typename T, uint64_t Capacity>
class LockFreeStack
{
public:
  using index_t = uint32_t;

  /// @brief creates and initalizes an empty LockFreeStack
  LockFreeStack() noexcept;

  LockFreeStack(const LockFreeStack &) = delete;
  LockFreeStack(LockFreeStack &&) = delete;
  LockFreeStack &operator=(const LockFreeStack &) = delete;
  LockFreeStack &operator=(LockFreeStack &&) = delete;

  /// @brief returns the capacity
  constexpr uint64_t capacity() noexcept;

  /// @brief tries to insert value in LIFO order
  /// @return true if insertion was successful (i.e. stack was not full during push), false otherwise
  bool try_push(const T &value) noexcept;

  /// @brief tries to remove value in LIFO order
  /// @return true if removal was successful (in this case value references the popped value)
  bool pop(T &value) noexcept;

  /// @brief tries to remove value in LIFO order
  /// @return value if removal was successful, empty optional otherwise
  std::optional<T> pop() noexcept;

  /// @brief check whether the stack is empty
  /// @return true if the stack is empty
  /// note that if the queue is used concurrently it might
  /// not be empty anymore after the call
  ///  (but it was at some point during the call)
  bool empty() noexcept;

private:
  using byte_t = uint8_t;

  IndexStack<Capacity, index_t> m_freeIndices;
  IndexStack<Capacity, index_t> m_usedIndices;

  //TODO create a buffer abstraction to be configured with desired size later
  //and make it configurable at init

  // m_buffer is large enough for Capacity elements of type T
  // must be aligned as T due to placement new construction
  alignas(alignof(T)) byte_t m_buffer[Capacity * sizeof(T)];

private:
  T *toPtr(index_t index) noexcept;
};

#include "lockfree_stack.inl"
