#pragma once

#include <cstddef>

template <typename T>
class RBTree
{
    enum class Color
    {
        RED,
        BLACK,
        DBLACK
    };

    enum class RotationDirection
    {
        LEFT, RIGHT
    };

    struct Node
    {
        T value;

        // Default color of the node is red
        Color color = Color::RED;

        Node *parent = nullptr;
        Node *lchild = nullptr;
        Node *rchild = nullptr;

#ifndef NDEBUG
        static std::size_t counter;
        Node()
        {
            counter++;
        }
        ~Node()
        {
            counter--;
        }
#endif // !NDEBUG
    };

    Node *root = nullptr;

    // Internal functions
    void delete_children(Node *parent);
    void rebalance(Node *node);

    // Helpers
    template <RBTree<T>::RotationDirection R>
    void __rotate_template(Node *node);

    void rotate_left(Node *node);
    void rotate_right(Node *node);
    Node *&getParentToSiblingPointer(const Node *child);
    Node *&getParentToChildPointer(const Node *child);

    // Assertions
#ifndef NDEBUG
    void check_children(Node *parent);
    void check_parent(Node *parent);
    void check_coloring(Node *parent);
    void check_depth(Node *parent);
    void __check_depth(Node *parent, const int &depth, int current_depth);
    static std::size_t counter;
#endif // !NDEBUG

public:
    RBTree() = default;
    ~RBTree();

    void add(const T &value);
    bool remove(const T &value);
    bool contains(const T &value) const;

    void print() const;
};

#ifndef NDEBUG
template <typename T>
std::size_t RBTree<T>::counter = 0;

template <typename T>
std::size_t RBTree<T>::Node::counter = 0;
#endif // !NDEBUG

// For template explicit instantiations
#include "RBTree.cpp"