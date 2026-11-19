#pragma once

#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "List.h"

 //Stores elements in a doubly linked list
 //with independently allocated nodes
 //Supports insertion and removal at both ends
 //without moving existing elements
template <typename T>
class LinkedList : public List<T>
{
private:
    // Holds one element and links to its neighbors
    struct Node
    {
        template <typename... Args>
        explicit Node(Args&&... args)
            : value(std::forward<Args>(args)...), previous(nullptr), next(nullptr)
        {
        }

        T value;
        Node* previous;
        Node* next;
    };

public:
    // Iterates over nodes in either a mutable or a read-only list
    template <bool IsConst>
    class BasicIterator
    {
        friend class LinkedList;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<IsConst, const T*, T*>;
        using reference = std::conditional_t<IsConst, const T&, T&>;

        BasicIterator() : current(nullptr), owner(nullptr) {}

        reference operator*() const { return current->value; }
        pointer operator->() const { return &current->value; }

        BasicIterator& operator++()
        {
            current = current->next;
            return *this;
        }

        BasicIterator operator++(int)
        {
            BasicIterator previous = *this;
            ++(*this);
            return previous;
        }

        BasicIterator& operator--()
        {
            current = current == nullptr ? owner->tail : current->previous;
            return *this;
        }

        BasicIterator operator--(int)
        {
            BasicIterator previous = *this;
            --(*this);
            return previous;
        }

        bool operator==(const BasicIterator& other) const
        {
            return current == other.current && owner == other.owner;
        }

        bool operator!=(const BasicIterator& other) const
        {
            return !(*this == other);
        }

    private:
        BasicIterator(Node* node, const LinkedList* list)
            : current(node), owner(list)
        {
        }

        Node* current;
        const LinkedList* owner;
    };

    using Iterator = BasicIterator<false>;
    using ConstIterator = BasicIterator<true>;
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

    // Creates an empty list
    LinkedList() = default;

    // Copies all elements into independently allocated nodes
    LinkedList(const LinkedList& other)
    {
        try
        {
            for (const T& value : other)
            {
                pushBack(value);
            }
        }
        catch (...)
        {
            clear();
            throw;
        }
    }

    // Transfers ownership of all nodes without copying
    LinkedList(LinkedList&& other) noexcept
        : head(std::exchange(other.head, nullptr)),
        tail(std::exchange(other.tail, nullptr)),
        count(std::exchange(other.count, 0))
    {
    }

    // Copies through a temporary list to preserve the original on failure
    LinkedList& operator=(const LinkedList& other)
    {
        if (this != &other)
        {
            LinkedList copy(other);
            swap(copy);
        }
        return *this;
    }

    // Replaces the current nodes with nodes taken from another list
    LinkedList& operator=(LinkedList&& other) noexcept
    {
        if (this != &other)
        {
            clear();
            head = std::exchange(other.head, nullptr);
            tail = std::exchange(other.tail, nullptr);
            count = std::exchange(other.count, 0);
        }
        return *this;
    }

    // Frees all nodes owned by the list
    ~LinkedList() override
    {
        clear();
    }

    // Adds an element to the end of the list
    void pushBack(const T& value) override { emplaceBack(value); }
    void pushBack(T&& value) override { emplaceBack(std::move(value)); }

    // Constructs an element directly at the end of the list
    template <typename... Args>
    T& emplaceBack(Args&&... args)
    {
        return emplace(count, std::forward<Args>(args)...);
    }

    // Adds an element to the beginning of the list
    void pushFront(const T& value) { insert(0, value); }
    void pushFront(T&& value) { insert(0, std::move(value)); }

    // Inserts an element before the specified index
    void insert(std::size_t index, const T& value) override
    {
        emplace(index, value);
    }

    void insert(std::size_t index, T&& value) override
    {
        emplace(index, std::move(value));
    }

    // Constructs an element directly before the specified index
    template <typename... Args>
    T& emplace(std::size_t index, Args&&... args)
    {
        if (index > count)
        {
            throw std::out_of_range("LinkedList insertion index is out of range");
        }

        Node* inserted = new Node(std::forward<Args>(args)...);

        if (index == count)
        {
            inserted->previous = tail;
            if (tail != nullptr)
            {
                tail->next = inserted;
            }
            else
            {
                head = inserted;
            }
            tail = inserted;
        }
        else
        {
            Node* following = nodeAt(index);
            inserted->previous = following->previous;
            inserted->next = following;
            following->previous = inserted;

            if (inserted->previous != nullptr)
            {
                inserted->previous->next = inserted;
            }
            else
            {
                head = inserted;
            }
        }

        ++count;
        return inserted->value;
    }

    // Removes the element at the specified index
    void erase(std::size_t index) override
    {
        if (index >= count)
        {
            throw std::out_of_range("LinkedList removal index is out of range");
        }
        removeNode(nodeAt(index));
    }

    // Removes an element while preserving the next iteration position
    Iterator erase(Iterator position)
    {
        if (position.owner != this || position.current == nullptr)
        {
            throw std::out_of_range("Invalid LinkedList iterator");
        }

        Node* following = position.current->next;
        removeNode(position.current);
        return Iterator(following, this);
    }

    // Removes the first or last element
    void popFront() { erase(0); }

    void popBack()
    {
        if (this->empty())
        {
            throw std::out_of_range("Cannot remove an element from an empty LinkedList");
        }
        erase(count - 1);
    }

    // Returns the number of stored elements
    std::size_t size() const noexcept override { return count; }

    // Returns an element by index, using the nearest end
    T& at(std::size_t index) override
    {
        if (index >= count)
        {
            throw std::out_of_range("LinkedList index is out of range");
        }
        return nodeAt(index)->value;
    }

    const T& at(std::size_t index) const override
    {
        if (index >= count)
        {
            throw std::out_of_range("LinkedList index is out of range");
        }
        return nodeAt(index)->value;
    }

    // Releases every node and restores an empty state
    void clear() noexcept override
    {
        Node* current = head;
        while (current != nullptr)
        {
            Node* following = current->next;
            delete current;
            current = following;
        }
        head = nullptr;
        tail = nullptr;
        count = 0;
    }

    // Provides forward and reverse iteration
    Iterator begin() noexcept { return Iterator(head, this); }
    Iterator end() noexcept { return Iterator(nullptr, this); }

    ConstIterator begin() const noexcept { return ConstIterator(head, this); }
    ConstIterator end() const noexcept { return ConstIterator(nullptr, this); }

    ConstIterator cbegin() const noexcept { return begin(); }
    ConstIterator cend() const noexcept { return end(); }

    ReverseIterator rbegin() noexcept { return ReverseIterator(end()); }
    ReverseIterator rend() noexcept { return ReverseIterator(begin()); }

    ConstReverseIterator rbegin() const noexcept { return ConstReverseIterator(end()); }
    ConstReverseIterator rend() const noexcept { return ConstReverseIterator(begin()); }

private:
    // Finds a node from the closer end of the list
    Node* nodeAt(std::size_t index) const noexcept
    {
        Node* current;

        if (index < count / 2)
        {
            current = head;
            for (std::size_t i = 0; i < index; ++i)
            {
                current = current->next;
            }
        }
        else
        {
            current = tail;
            for (std::size_t i = count - 1; i > index; --i)
            {
                current = current->previous;
            }
        }

        return current;
    }

    // Unlinks and destroys an existing node
    void removeNode(Node* node) noexcept
    {
        if (node->previous != nullptr)
        {
            node->previous->next = node->next;
        }
        else
        {
            head = node->next;
        }

        if (node->next != nullptr)
        {
            node->next->previous = node->previous;
        }
        else
        {
            tail = node->previous;
        }

        delete node;
        --count;
    }

    // Exchanges ownership of nodes with another list
    void swap(LinkedList& other) noexcept
    {
        std::swap(head, other.head);
        std::swap(tail, other.tail);
        std::swap(count, other.count);
    }

    Node* head = nullptr;
    Node* tail = nullptr;
    std::size_t count = 0;
};
