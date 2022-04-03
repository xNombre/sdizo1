#pragma once

template <typename T>
class List
{
public:
    struct Node
    {
        T val;
        Node *prev = nullptr;
        Node *next = nullptr;
    };

    List() = default;
    ~List();

    void push_front(const T &val);
    void push_back(const T &val);
    void insert(const T &val, Node *&at);

    // Find first node with matching value
    Node *get_node(const T &val) const;

    // Returns true if value was present in the list
    bool remove(const T &val);
    bool remove(Node *&node);
    void pop_front() volatile;
    void pop_back();

    bool contains(const T &val) const;
    void print() const;

private:
    Node *head = nullptr;
    Node *tail = nullptr;
};

// For template explicit instantiations
#include "List.cpp"