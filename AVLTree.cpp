#pragma once

#include <iostream>
#include <queue>

#include "AVLTree.hpp"

#define RST  "\x1B[0m"
#define KRED  "\x1B[41m"
#define KBLU  "\x1B[44m"

template <typename T>
void AVLTree<T>::delete_children(Node *&node)
{
    if (node->lchild)
        delete_children(node->lchild);
    if (node->rchild)
        delete_children(node->rchild);

    delete node;
}

template <typename T>
AVLTree<T>::~AVLTree()
{
    if (root)
        delete_children(root);
}

template <typename T>
inline auto AVLTree<T>::getParentToChildPointer(const AVLTree<T>::Node *child) -> AVLTree<T>::Node *&
{
    return (child == root ? root :
            (child->parent->lchild == child ? child->parent->lchild : child->parent->rchild));
}

template <typename T>
inline auto AVLTree<T>::getParentToSiblingPointer(const AVLTree<T>::Node *child) -> AVLTree<T>::Node *&
{
    return (child->parent->lchild == child ? child->parent->rchild : child->parent->lchild);
}

template <typename T>
template <typename AVLTree<T>::RotationDirection R>
void AVLTree<T>::__rotate_template(AVLTree<T>::Node *node)
{
    Node *parent = node->parent;

    // During rotation one of the children needs to be moved
    // from node to parent, store it in a temporary variable
    Node **child_swap_from, **child_swap_to, *child_swap;

    if constexpr (R == RotationDirection::LEFT) {
        child_swap_from = &node->lchild;
        child_swap_to = &parent->rchild;
    }
    else {
        child_swap_from = &node->rchild;
        child_swap_to = &parent->lchild;
    }

    child_swap = *child_swap_from;

    *child_swap_from = parent;
    node->parent = parent->parent;

    getParentToChildPointer(parent) = node;
    parent->parent = node;

    *child_swap_to = child_swap;
    if (child_swap)
        child_swap->parent = parent;

    parent->fixHeight();
    node->fixHeight();
}

template <typename T>
void AVLTree<T>::rotate_left(Node *node)
{
    __rotate_template<RotationDirection::LEFT>(node);
}

template <typename T>
void AVLTree<T>::rotate_right(Node *node)
{
    __rotate_template<RotationDirection::RIGHT>(node);
}

template <typename T>
void AVLTree<T>::add(const T &value)
{
    if (!root) {
        Node *node = new Node();
        node->value = value;
        root = node;

        return;
    }

    // Binary search
    Node **search = &root;
    Node *parent;

    while (*search) {
        parent = *search;

        if (parent->value > value)
            search = &parent->lchild;
        else
            search = &parent->rchild;
    }

    // Add new node to the tree
    Node *node = new Node();
    node->value = value;
    node->parent = parent;

    *search = node;

    // Update height and rebalance if needed
    do {
        Node *heavyChild = node;
        node = node->parent;
        bool isLchild = node->lchild == heavyChild;

        node->fixHeight();

#ifndef NDEBUG
        if (abs(node->getBalance()) > 2)
            throw std::runtime_error("nope");
#endif // !NDEBUG

        if (abs(node->getBalance()) > 1) {
            // Special case
            {
                std::size_t lheight = 0, rheight = 0;
                if (heavyChild->lchild) {
                    lheight = heavyChild->lchild->height;
                }
                if (heavyChild->rchild) {
                    rheight = heavyChild->rchild->height;
                }

                if (isLchild && lheight < rheight) {
                    rotate_left(heavyChild->rchild);
                    heavyChild = node->lchild;
                }
                else if (!isLchild && rheight < lheight) {
                    rotate_right(heavyChild->lchild);
                    heavyChild = node->rchild;
                }
            }

            if (isLchild)
                rotate_right(heavyChild);
            else
                rotate_left(heavyChild);

            node = heavyChild;
        }

    } while (node != root);

#ifndef NDEBUG
    checkHeight(root);
#endif // !NDEBUG
}

template <typename T>
bool AVLTree<T>::remove(const T &value)
{
    Node *search = root;
    while (search && search->value != value) {
        if (search->value > value)
            search = search->lchild;
        else
            search = search->rchild;
    }

    if (!search)
        return false;

    auto children_count = (search->lchild ? 1 : 0) + (search->rchild ? 1 : 0);

    Node *nodeToBeRemoved = search;
    search = search->parent;

    if (children_count == 2) {
        Node *largest_lchild = nodeToBeRemoved->lchild;

        while (largest_lchild->rchild)
            largest_lchild = largest_lchild->rchild;

        nodeToBeRemoved->value = largest_lchild->value;
        nodeToBeRemoved = largest_lchild;

        if (largest_lchild->lchild) {
            largest_lchild->lchild->parent = largest_lchild->parent;
            getParentToChildPointer(largest_lchild) = largest_lchild->lchild;
        }
        else
            getParentToChildPointer(nodeToBeRemoved) = nullptr;

        search = largest_lchild->parent;
    }
    else if (children_count == 1) {
        auto &child_pointer = getParentToChildPointer(nodeToBeRemoved);

        child_pointer = nodeToBeRemoved->lchild ? nodeToBeRemoved->lchild : nodeToBeRemoved->rchild;
        child_pointer->parent = nodeToBeRemoved->parent;

        search = child_pointer;
    }
    else
        getParentToChildPointer(nodeToBeRemoved) = nullptr;

    if (!root) {
        delete nodeToBeRemoved;
        goto out;
    }

    delete nodeToBeRemoved;

    if (search == root) {
        search->fixHeight();
        goto out;
    }

    // Update height and rebalance if needed
    while (search != root) {
        search->fixHeight();

#ifndef NDEBUG
        if (abs(search->getBalance()) > 2)
            throw std::runtime_error("nope");
#endif // !NDEBUG

        if (abs(search->getBalance()) > 1) {
            Node *heavyChild = search->getBalance() > 0 ? search->rchild : search->lchild;
            bool isLchild = search->lchild == heavyChild;

            // Special case
            {
                std::size_t lheight = 0, rheight = 0;
                if (heavyChild->lchild) {
                    lheight = heavyChild->lchild->height;
                }
                if (heavyChild->rchild) {
                    rheight = heavyChild->rchild->height;
                }

                if (isLchild && lheight < rheight) {
                    rotate_left(heavyChild->rchild);
                    heavyChild = search->lchild;
                }
                else if (!isLchild && rheight < lheight) {
                    rotate_right(heavyChild->lchild);
                    heavyChild = search->rchild;
                }
            }

            if (isLchild)
                rotate_right(heavyChild);
            else
                rotate_left(heavyChild);

            search = heavyChild;
        }

        search = search->parent;
    }

    root->fixHeight();

out:
#ifndef NDEBUG
    checkHeight(root);
#endif // !NDEBUG

    return true;
}

template <typename T>
bool AVLTree<T>::contains(const T &value) const
{
    const Node *search = root;
    while (search && search->value != value) {
        if (search->value > value)
            search = search->lchild;
        else
            search = search->rchild;
    }

    if (search)
        return true;

    return false;
}

template <typename T>
void AVLTree<T>::print() const
{
    if (!root)
        return;

    std::queue<Node *> kolejka, kolejka2;

    Node *empty = new Node();

    int pow = 1, cur_pow = 0;

    kolejka.push(root);
    kolejka2.push(root);

    while (!kolejka2.empty()) {
        if (kolejka.front() == empty)
            std::cout << "N";
        else if (kolejka.front()->getBalance() > 0)
            std::cout << KRED << kolejka.front()->value << RST;
        else if (kolejka.front()->getBalance() < 0)
            std::cout << KBLU << kolejka.front()->value << RST;
        else
            std::cout << kolejka.front()->value;

        std::cout << " ";

        if (++cur_pow == pow) {
            std::cout << std::endl;
            cur_pow = 0;
            pow *= 2;
        }

        if (kolejka.front()->lchild) {
            kolejka.push(kolejka.front()->lchild);
            kolejka2.push(kolejka.front()->lchild);
        }
        else
            kolejka.push(empty);

        if (kolejka.front()->rchild) {
            kolejka.push(kolejka.front()->rchild);
            kolejka2.push(kolejka.front()->rchild);
        }
        else
            kolejka.push(empty);

        if (kolejka.front() != empty)
            kolejka2.pop();

        kolejka.pop();
    }

    if (cur_pow != 0)
        while (cur_pow++ < pow)
            std::cout << "N ";

    std::cout << std::endl;

    delete empty;
}

#ifndef NDEBUG
template <typename T>
std::size_t AVLTree<T>::checkHeight(Node *node)
{
    if (!node)
        return 0;

    std::size_t ret = 1 + std::max(checkHeight(node->lchild), checkHeight(node->rchild));
    if (ret != node->height)
        throw std::runtime_error("nope");

    return ret;
}
#endif // NDEBUG