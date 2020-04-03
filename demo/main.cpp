#include <iostream>

#include "index_stack.hpp"
#include "lockfree_stack.hpp"

//playground, work in progress

int main(void)
{
    {
        IndexStack<4> stack;

        stack.push(0);
        stack.push(1);
        stack.push(2);

        uint32_t value;
        if (stack.pop(value))
        {
            std::cout << "popped " << value << std::endl;
        }
        else
        {
            std::cout << "popped nothing" << std::endl;
        }

        if (stack.pop(value))
        {
            std::cout << "popped " << value << std::endl;
        }
        else
        {
            std::cout << "popped nothing" << std::endl;
        }

        if (stack.pop(value))
        {
            std::cout << "popped " << value << std::endl;
        }
        else
        {
            std::cout << "popped nothing" << std::endl;
        }
    }

    {
        std::cout << "**************" << std::endl;
        IndexStack<4> stack(IndexStack<4>::ConstructFull::Policy);
        uint32_t value;
        while (stack.pop(value))
        {
            std::cout << "popped " << value << std::endl;
        }
    }

    std::cout << "**************" << std::endl;
    LockFreeStack<int, 4> stack;
    stack.try_push(1ll);
    stack.try_push(2ll);
    stack.try_push(3ll);

    while (true)
    {
        auto popped = stack.pop();
        if (!popped)
            break;
        std::cout << "popped " << *popped << std::endl;
    }

    return 0;
}