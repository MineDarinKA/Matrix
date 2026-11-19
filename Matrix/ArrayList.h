#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>
#include "List.h"

// Stores elements in a dynamically allocated contiguous array
// Expands capacity when more elements are needed
template <typename T>
class ArrayList : public List<T>
{
private:
    using Allocator = std::allocator<T>;
    using Traits = std::allocator_traits<Allocator>;

public:
    using Iterator = T*;
    using ConstIterator = const T*;

    // Creates an empty array without allocating storage
    ArrayList() = default;

    // Creates an independent copy of all elements
    ArrayList(const ArrayList& other)
    {
        try
        {
            reserve(other.count);

            for (std::size_t i = 0; i < other.count; ++i)
            {
                pushBack(other.data[i]);
            }
        }
        catch (...)
        {
            release();
            throw;
        }
    }

    // Takes ownership of another array without copying elements
    ArrayList(ArrayList&& other) noexcept
        : data(std::exchange(other.data, nullptr)),
        count(std::exchange(other.count, 0)),
        capacity(std::exchange(other.capacity, 0))
    {
    }

    // Copies through a temporary array to protect the original
    ArrayList& operator=(const ArrayList& other)
    {
        if (this != &other)
        {
            ArrayList copy(other);
            swap(copy);
        }

        return *this;
    }

    // Releases the current storage and takes ownership of another array
    ArrayList& operator=(ArrayList&& other) noexcept
    {
        if (this != &other)
        {
            release();

            data = std::exchange(other.data, nullptr);
            count = std::exchange(other.count, 0);
            capacity = std::exchange(other.capacity, 0);
        }

        return *this;
    }

    // Destroys stored elements and releases the allocated memory
    ~ArrayList() override
    {
        release();
    }

    // Adds a copy or a moved element to the end
    void pushBack(const T& value) override
    {
        emplaceBack(value);
    }

    void pushBack(T&& value) override
    {
        emplaceBack(std::move(value));
    }

    // Appends an element using arguments for its constructor
    template <typename... Args>
    T& emplaceBack(Args&&... args)
    {
        if (count == capacity)
        {
            // Construct first in case an argument refers to an existing element
            T value(std::forward<Args>(args)...);

            reserve(nextCapacity());

            Traits::construct(allocator, data + count, std::move(value));
        }
        else
        {
            Traits::construct(
                allocator,
                data + count,
                std::forward<Args>(args)...
            );
        }

        ++count;
        return data[count - 1];
    }

    // Inserts a copy or a moved element at the requested index
    void insert(std::size_t index, const T& value) override
    {
        emplace(index, value);
    }

    void insert(std::size_t index, T&& value) override
    {
        emplace(index, std::move(value));
    }

    // Inserts a new element and shifts following elements to the right
    template <typename... Args>
    T& emplace(std::size_t index, Args&&... args)
    {
        if (index > count)
        {
            throw std::out_of_range(
                "ArrayList insertion index is out of range"
            );
        }

        if (index == count)
        {
            return emplaceBack(std::forward<Args>(args)...);
        }

        // Preserve the value before reallocation or shifting
        T value(std::forward<Args>(args)...);

        if (count == capacity)
        {
            reserve(nextCapacity());
        }

        // Construct a new last element before shifting existing values
        Traits::construct(
            allocator,
            data + count,
            std::move_if_noexcept(data[count - 1])
        );

        ++count;

        for (std::size_t i = count - 2; i > index; --i)
        {
            data[i] = std::move_if_noexcept(data[i - 1]);
        }

        data[index] = std::move(value);

        return data[index];
    }

    // Removes one element and shifts following values to the left
    void erase(std::size_t index) override
    {
        if (index >= count)
        {
            throw std::out_of_range(
                "ArrayList removal index is out of range"
            );
        }

        for (std::size_t i = index; i + 1 < count; ++i)
        {
            data[i] = std::move_if_noexcept(data[i + 1]);
        }

        Traits::destroy(allocator, data + count - 1);
        --count;
    }

    // Removes a valid iterator range and returns the following position
    Iterator erase(Iterator first, Iterator last)
    {
        if (first == last)
        {
            return first;
        }

        const std::size_t index =
            static_cast<std::size_t>(first - data);

        const std::size_t removed =
            static_cast<std::size_t>(last - first);

        for (std::size_t i = index; i + removed < count; ++i)
        {
            data[i] = std::move_if_noexcept(data[i + removed]);
        }

        for (std::size_t i = count - removed; i < count; ++i)
        {
            Traits::destroy(allocator, data + i);
        }

        count -= removed;

        return data + index;
    }

    // Returns the current element count and allocated capacity
    std::size_t size() const noexcept override
    {
        return count;
    }

    std::size_t getCapacity() const noexcept
    {
        return capacity;
    }

    // Provides bounds-checked access to the elements
    T& at(std::size_t index) override
    {
        if (index >= count)
        {
            throw std::out_of_range(
                "ArrayList index is out of range"
            );
        }

        return data[index];
    }

    const T& at(std::size_t index) const override
    {
        if (index >= count)
        {
            throw std::out_of_range(
                "ArrayList index is out of range"
            );
        }

        return data[index];
    }

    // Provides unchecked element access for known valid indices
    T& operator[](std::size_t index) noexcept
    {
        return data[index];
    }

    const T& operator[](std::size_t index) const noexcept
    {
        return data[index];
    }

    // Destroys all elements while keeping the allocated storage
    void clear() noexcept override
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            Traits::destroy(allocator, data + i);
        }

        count = 0;
    }

    // Allocates enough storage without changing the element count
    void reserve(std::size_t newCapacity)
    {
        if (newCapacity <= capacity)
        {
            return;
        }

        if (newCapacity > Traits::max_size(allocator))
        {
            throw std::length_error(
                "ArrayList capacity is too large"
            );
        }

        T* expanded = Traits::allocate(allocator, newCapacity);
        std::size_t constructed = 0;

        try
        {
            for (; constructed < count; ++constructed)
            {
                Traits::construct(
                    allocator,
                    expanded + constructed,
                    std::move_if_noexcept(data[constructed])
                );
            }
        }
        catch (...)
        {
            for (std::size_t i = 0; i < constructed; ++i)
            {
                Traits::destroy(allocator, expanded + i);
            }

            Traits::deallocate(allocator, expanded, newCapacity);
            throw;
        }

        releaseStorage();

        data = expanded;
        capacity = newCapacity;
    }

    // Exposes contiguous elements for standard sorting algorithms
    Iterator begin() noexcept
    {
        return data;
    }

    Iterator end() noexcept
    {
        return data == nullptr ? nullptr : data + count;
    }

    ConstIterator begin() const noexcept
    {
        return data;
    }

    ConstIterator end() const noexcept
    {
        return data == nullptr ? nullptr : data + count;
    }

private:
    // Chooses a larger capacity while preventing integer overflow
    std::size_t nextCapacity() const
    {
        const std::size_t maximum = Traits::max_size(allocator);

        if (capacity == maximum)
        {
            throw std::length_error(
                "ArrayList cannot grow further"
            );
        }

        if (capacity == 0)
        {
            return maximum < 4 ? maximum : 4;
        }

        return capacity > maximum / 2 ?
            maximum : capacity * 2;
    }

    // Destroys all objects and releases the current allocation
    void release() noexcept
    {
        clear();

        if (data != nullptr)
        {
            Traits::deallocate(allocator, data, capacity);

            data = nullptr;
            capacity = 0;
        }
    }

    // Releases old storage without modifying the element count
    void releaseStorage() noexcept
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            Traits::destroy(allocator, data + i);
        }

        if (data != nullptr)
        {
            Traits::deallocate(allocator, data, capacity);
        }
    }

    // Exchanges storage ownership with another array
    void swap(ArrayList& other) noexcept
    {
        std::swap(data, other.data);
        std::swap(count, other.count);
        std::swap(capacity, other.capacity);
    }

    Allocator allocator;
    T* data = nullptr;
    std::size_t count = 0;
    std::size_t capacity = 0;
};
