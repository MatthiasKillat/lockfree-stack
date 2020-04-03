// Copyright (c) 2020 by Matthias Killat. All rights reserved.

#pragma once

#include "stdint.h"
#include <atomic>

//TODO: work in progress

//TODO: experimental, tests and proof of algorithmic correctness are missing
//since it was just an afternoons thought there surely will be correctness issues:-)

template <uint64_t Capacity, typename index_t = uint32_t>
class IndexStack
{
private:
  struct Top
  {
    constexpr Top(index_t index = 0, index_t id = 0) noexcept : index(index), id(id) {}
    index_t index;
    index_t id;
  };

  static_assert(std::atomic<Top>::is_always_lock_free);

  index_t m_values[Capacity];
  std::atomic<Top> m_top;

public:
  IndexStack(const IndexStack &) = delete;
  IndexStack(IndexStack &&) = delete;
  IndexStack &operator=(const IndexStack &) = delete;
  IndexStack &operator=(IndexStack &&) = delete;

  // just to distingish between constructors at compile time and make the
  // construction policy more explicit
  enum class ConstructFull
  {
    Policy
  };

  enum class ConstructEmpty
  {
    Policy
  };

  /// @brief constructs an empty IndexStack
  IndexStack(ConstructEmpty policy = ConstructEmpty::Policy);

  /// @brief constructs IndexStack filled with all indices 0,1,...capacity-1
  IndexStack(ConstructFull);

  /// @brief get the capacity of the IndexStack
  /// @return capacity of the IndexStack
  constexpr uint64_t capacity();

  /// @brief push index into the stack in LIFO order
  /// constraint: pushing more indices than capacity is not allowed
  /// constraint: only indices in the range [0, Capacity-1] are allowed
  void push(index_t value);

  /// @brief tries to remove index in LIFO order
  /// @return true iff removal was successful (i.e. stack was not empty)
  /// value is only valid if the function returns true
  bool pop(index_t &value);

  /// @brief check whether the stack is empty
  /// @return true iff the stack is empty
  /// note that if the stack is used concurrently it might not be empty
  /// anymore after the call (but it was at some point during the call)
  bool empty();
};

#include "index_stack.inl"
