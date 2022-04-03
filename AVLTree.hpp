#pragma once

template<typename T>
class AVLTree {
    enum class RotationDirection
    {
        LEFT, RIGHT
    };

    struct Node {
        T value;
        std::size_t height = 1;

        Node *parent = nullptr;
        Node *lchild = nullptr;
        Node *rchild = nullptr;

        int getBalance() const
        {
            return (rchild ? (long long)rchild->height : 0) -
                (lchild ? (long long)lchild->height : 0);
        }
        void fixHeight()
        {
            height = std::max(rchild ? (long long)rchild->height : 0,
                              lchild ? (long long)lchild->height : 0) + 1;
        }
    };

    Node *root = nullptr;

    void delete_children(Node *&node);

    // Helpers
    template <AVLTree<T>::RotationDirection R>
    void __rotate_template(Node *node);

    void rotate_left(Node *node);
    void rotate_right(Node *node);
    Node *&getParentToSiblingPointer(const Node *child);
    Node *&getParentToChildPointer(const Node *child);

    std::size_t checkHeight(Node *node);

public:
    AVLTree() = default;
    ~AVLTree();

    void add(const T &value);
    bool remove(const T &value);
    bool contains(const T &value) const;

    void print() const;
};

// For templates explicit instantiations
#include "AVLTree.cpp"