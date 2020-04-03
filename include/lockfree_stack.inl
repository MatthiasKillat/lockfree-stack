// Copyright (c) 2020 by Matthias Killat. All rights reserved.

template <typename T, uint32_t Capacity>
LockFreeStack<T, Capacity>::LockFreeStack() noexcept
    : m_freeIndices(IndexStack<Capacity>::ConstructFull::Policy), m_usedIndices(IndexStack<Capacity>::ConstructEmpty::Policy)
{
}

template <typename T, uint32_t Capacity>
constexpr uint32_t LockFreeStack<T, Capacity>::capacity() noexcept
{
    return Capacity;
}

template <typename T, uint32_t Capacity>
bool LockFreeStack<T, Capacity>::try_push(const T &value) noexcept
{
    index_t index;
    if (!m_freeIndices.pop(index))
    {
        return false; // detected full stack
    }

    auto ptr = toPtr(index);
    new (ptr) T(value);

    m_usedIndices.push(index);

    return true;
}

template <typename T, uint32_t Capacity>
bool LockFreeStack<T, Capacity>::pop(T &value) noexcept
{
    index_t index;
    if (!m_usedIndices.pop(index))
    {
        return false; // empty
    }

    auto ptr = toPtr(index);
    value = std::move(*ptr);
    ptr->~T();

    m_freeIndices.push(index);
    return true;
}

template <typename T, uint32_t Capacity>
std::optional<T> LockFreeStack<T, Capacity>::pop() noexcept
{
    //avoid requiring a ctor
    alignas(alignof(T)) byte_t buffer[sizeof(T)];
    auto ptr = reinterpret_cast<T *>(buffer);
    if (pop(*ptr))
    {
        return *ptr;
    }
    return std::nullopt;
}

template <typename T, uint32_t Capacity>
bool LockFreeStack<T, Capacity>::empty() noexcept
{
    return m_usedIndices.empty();
}

template <typename T, uint32_t Capacity>
T *LockFreeStack<T, Capacity>::toPtr(index_t index) noexcept
{
    auto p = &(m_buffer[index * sizeof(T)]);
    return reinterpret_cast<T *>(p);
}

template <typename T, uint32_t Capacity>
std::vector<T> LockFreeStack<T, Capacity>::multipop(index_t count)
{
    auto indices = m_usedIndices.multipop(count);
    if (indices.empty())
    {
        return std::vector<T>();
    }

    std::vector<T> result;
    result.reserve(indices.size());

    for (auto &index : indices)
    {

        auto ptr = toPtr(index);
        result.push_back(std::move(*ptr));
        ptr->~T();

        m_freeIndices.push(index);
    }

    return result;
}