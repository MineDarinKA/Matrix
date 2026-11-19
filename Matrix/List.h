#pragma once

#include <cstddef>

// Defines a common interface for template list containers
template <typename T>
class List
{
public:
    // Allows proper destruction through a base class pointer
    virtual ~List() = default;

    // Adds a copy of an element to the end of the list
    virtual void pushBack(const T& value) = 0;

    // Moves an element to the end of the list
    virtual void pushBack(T&& value) = 0;

    // Inserts a copy of an element at the specified index
    virtual void insert(std::size_t index, const T& value) = 0;

    // Moves an element into the specified index
    virtual void insert(std::size_t index, T&& value) = 0;

    // Removes the element at the specified index
    virtual void erase(std::size_t index) = 0;

    // Returns the current number of elements
    virtual std::size_t size() const = 0;

    // Returns a modifiable reference to an element
    virtual T& at(std::size_t index) = 0;

    // Returns a read-only reference to an element
    virtual const T& at(std::size_t index) const = 0;

    // Removes all elements from the list
    virtual void clear() = 0;

    // Checks whether the list contains no elements
    bool empty() const
    {
        return size() == 0;
    }
};
