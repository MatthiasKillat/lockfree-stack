// Copyright (c) 2020 by Matthias Killat. All rights reserved.
#pragma once

#include "index_stack.hpp"
#include <optional>

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
  /// @return true iff insertion was successful (i.e. stack was not full during push), false otherwise
  bool try_push(const T &value) noexcept;

  /// @brief tries to remove value in LIFO order
  /// @return true iff removal was successful (in this case value references the popped value)
  bool pop(T &value) noexcept;

  /// @brief tries to remove value in LIFO order
  /// @return value iff removal was successful, empty optional otherwise
  std::optional<T> pop() noexcept;

  /// @brief check whether the stack is empty
  /// @return true iff the stack is empty
  /// note that if the queue is used concurrently it might
  /// not be empty anymore after the call
  ///  (but it was at some point during the call)
  bool empty() noexcept;

private:
  using byte_t = uint8_t;

  // actually m_freeIndices do not have to be in a queue, it could be another
  // multi-push multi-pop capable lockfree container (e.g. a stack or a list)
  /// @todo: replace with more efficient lockfree structure once available
  IndexStack<Capacity, index_t> m_freeIndices;

  // required to be a queue for LockFreeStack to exhibit FIFO behaviour
  IndexStack<Capacity, index_t> m_usedIndices;

  /// @todo create a buffer abstraction to be configured with desired size later
  // m_buffer is large enough for Capacity elements of type T
  // must be aligned as T due to placement new construction
  alignas(alignof(T)) byte_t m_buffer[Capacity * sizeof(T)];

  //TODO abstract this buffer
  //and make it configurable at init

private:
  T *toPtr(index_t index) noexcept;
};

#include "lockfree_stack.inl"
