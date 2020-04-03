// Copyright (c) 2020 by Matthias Killat. All rights reserved.

#include <algorithm>

template <uint64_t Capacity, typename index_t>
IndexStack<Capacity, index_t>::IndexStack(ConstructEmpty)
    : m_top{Top{Capacity}}
{
}

template <uint64_t Capacity, typename index_t>
IndexStack<Capacity, index_t>::IndexStack(ConstructFull)
    : m_top{Top{0}}
{
    for (index_t i = 0; i < Capacity; ++i)
    {
        m_values[i] = i;
    }
}

template <uint64_t Capacity, typename index_t>
constexpr uint64_t IndexStack<Capacity, index_t>::capacity()
{
    return Capacity;
}

template <uint64_t Capacity, typename index_t>
void IndexStack<Capacity, index_t>::push(index_t value)
{
    do
    {
        auto oldTop = m_top.load();
        //value is unique and the current call owns it, hence it can be used as marker
        Top announceTop(oldTop.index, value);

        //we intend to write value and announce that via this compare exchange
        //before actually writing data
        //note that this will block no one from also trying to write or read

        //spurious failures are ok, retry is cheap from here
        if (!m_top.compare_exchange_weak(oldTop, announceTop))
        {
            continue; //retry
        }

        Top newTop(announceTop.index - 1, 0);
        m_values[newTop.index] = value;

        //we do not want to spuriously fail here after the expensive first CAS
        //if this fails, we must assume that someone else also wrote a value and retry
        if (m_top.compare_exchange_strong(announceTop, newTop))
        {
            break;
        }
        //either the top index changed or someone else intends to write
        //some other value, retry

    } while (true);
}

template <uint64_t Capacity, typename index_t>
bool IndexStack<Capacity, index_t>::pop(index_t &value)
{
    do
    {
        auto oldTop = m_top.load();
        if (oldTop.index == Capacity)
        {
            return false; //empty
        }
        value = m_values[oldTop.index];
        Top announceTop(oldTop.index, value);

        //we intend to read value
        if (!m_top.compare_exchange_weak(oldTop, announceTop))
        {
            continue;
        }
        Top newTop(announceTop.index + 1, announceTop.id);

        //check whether the top index changes or someone intends to
        //read or write a different value
        //if, due to concurrent pushes and pops
        //value is popped and ends up at the same position (index) again when we
        //check, it is no problem (operations and data are still consistent)
        if (m_top.compare_exchange_strong(announceTop, newTop))
        {
            break;
        }

    } while (true);

    return true;
}

template <uint64_t Capacity, typename index_t>
bool IndexStack<Capacity, index_t>::empty()
{
    return m_top.load().index == Capacity;
}

template <uint64_t Capacity, typename index_t>
std::list<index_t> IndexStack<Capacity, index_t>::multipop(index_t count)
{
    std::list<index_t> result;
    do
    {
        auto oldTop = m_top.load();
        if (oldTop.index == Capacity)
        {
            return result; //empty
        }

        auto value = m_values[oldTop.index];
        Top announceTop(oldTop.index, value);

        //we intend to read value
        if (!m_top.compare_exchange_weak(oldTop, announceTop))
        {
            continue;
        }

        //are enough elements available?

        //TODO: let capacity be of the same type as index_t
        auto numToPop = std::min(count, static_cast<index_t>(Capacity - oldTop.index));

        //; (count < Capacity - oldTop.index) ? count : Capacity - oldTop.index;
        //we need to store these values before we move the head
        for (index_t i = oldTop.index; i < oldTop.index + numToPop; ++i)
        {
            result.push_back(m_values[i]);
        }

        Top newTop(announceTop.index + numToPop, announceTop.id);

        //check whether the top index changes or someone intends to
        //read or write a different value
        //if, due to concurrent pushes and pops
        //value is popped and ends up at the same position (index) again when we
        //check, it is no problem (operations and data are still consistent)
        if (m_top.compare_exchange_strong(announceTop, newTop))
        {
            break;
        }

        result.clear(); //retry, clear previous results

    } while (true);

    return result;
}