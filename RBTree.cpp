#pragma once

#include <iostream>
#include <queue>

#include "RBTree.hpp"

#define RST  "\x1B[0m"
#define KRED  "\x1B[41m"

template <typename T>
void RBTree<T>::delete_children(Node *parent)
{
    // Recursively delete all children of a parent
    if (parent->lchild)
        delete_children(parent->lchild);
    if (parent->rchild)
        delete_children(parent->rchild);

    delete parent;

#ifndef NDEBUG
    counter--;
#endif // NDEBUG
}

template <typename T>
RBTree<T>::~RBTree()
{
    if (root)
        delete_children(root);
}

template <typename T>
void RBTree<T>::add(const T &value)
{
    if (!root) {
        Node *node = new Node;
        node->value = value;

        // Root is always black
        node->color = Color::BLACK;
        root = node;

#ifndef NDEBUG
        counter++;
#endif // !NDEBUG

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

    // Create new node and add it to the tree
    Node *node = new Node;
    node->value = value;
    node->parent = parent;

    *search = node;

    rebalance(node);

#ifndef NDEBUG
    counter++;
    if (counter != Node::counter)
        throw std::runtime_error("nope");
#endif // !NDEBUG
}

template <typename T>
inline auto RBTree<T>::getParentToChildPointer(const RBTree<T>::Node *child) -> RBTree<T>::Node *&
{
    return (child == root ? root :
            (child->parent->lchild == child ? child->parent->lchild : child->parent->rchild));
}

template <typename T>
inline auto RBTree<T>::getParentToSiblingPointer(const RBTree<T>::Node *child) -> RBTree<T>::Node *&
{
    return (child->parent->lchild == child ? child->parent->rchild : child->parent->lchild);
}

template <typename T>
template <typename RBTree<T>::RotationDirection R>
void RBTree<T>::__rotate_template(RBTree<T>::Node *node)
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
}

template <typename T>
void RBTree<T>::rotate_left(Node *node)
{
    __rotate_template<RotationDirection::LEFT>(node);
}

template <typename T>
void RBTree<T>::rotate_right(Node *node)
{
    __rotate_template<RotationDirection::RIGHT>(node);
}

template <typename T>
void RBTree<T>::rebalance(Node *node)
{
    while (node != root && node->parent->color == Color::RED) {
        Node *parent = node->parent;
        Node *grandparent = parent->parent;
        Node *parent_sibling = getParentToSiblingPointer(parent);

        if (!parent_sibling || parent_sibling->color == Color::BLACK) {
            // Uncle is black/null

            // Swap node with parent if node's value is between parent and grandparent
            // This is a special case preliminary to actual tree rotation
            if (parent->value < node->value && grandparent->value > node->value) {
                // Parent is lchild
                rotate_left(node);

                node = node->lchild;
                parent = node->parent;
                grandparent = parent->parent;
            }
            else if (parent->value > node->value && grandparent->value < node->value) {
                // Parent is rchild
                rotate_right(node);

                node = node->rchild;
                parent = node->parent;
                grandparent = parent->parent;
            }

            // Fix coloring
            parent->color = Color::BLACK;
            node->color = Color::RED;
            grandparent->color = Color::RED;

            // Now rotate the tree
            if (parent->lchild == node)
                rotate_right(parent);
            else
                rotate_left(parent);

            break;
        }

        // Uncle is red
        parent->color = Color::BLACK;
        parent_sibling->color = Color::BLACK;
        grandparent->color = Color::RED;
        node = grandparent;
    }

    // Root is always black
    root->color = Color::BLACK;

#ifndef NDEBUG
    check_children(root);
    check_parent(root);
    check_coloring(root);
    check_depth(root);
#endif // !NDEBUG
}

template <typename T>
bool RBTree<T>::remove(const T &value)
{
    // Binary search
    Node *nodeToBeRemoved = root;
    while (nodeToBeRemoved && nodeToBeRemoved->value != value) {
        if (nodeToBeRemoved->value > value)
            nodeToBeRemoved = nodeToBeRemoved->lchild;
        else
            nodeToBeRemoved = nodeToBeRemoved->rchild;
    }

    if (!nodeToBeRemoved)
        return false;

    int children_count;
    Node *dbFix;
    bool nodeIsRed = false;

    if (nodeToBeRemoved == root && !root->lchild && !root->rchild) {
        root = nullptr;
        goto out;
    }

    children_count = (nodeToBeRemoved->lchild ? 1 : 0) +
        (nodeToBeRemoved->rchild ? 1 : 0);

    // If a node has two children then replace it with its largest lchild
    if (children_count == 2) {
        Node *largest_lchild = nodeToBeRemoved->lchild;

        while (largest_lchild->rchild)
            largest_lchild = largest_lchild->rchild;

        nodeToBeRemoved->value = largest_lchild->value;
        nodeToBeRemoved = largest_lchild;

        children_count = (nodeToBeRemoved->lchild ? 1 : 0) +
            (nodeToBeRemoved->rchild ? 1 : 0);
    }

    // If node is red, rebalancing is not needed
    if (nodeToBeRemoved->color == Color::RED) {
        if (children_count == 0) {
            getParentToChildPointer(nodeToBeRemoved) = nullptr;
            goto out;
        }
        nodeIsRed = true;
    }

    // Parent or child is red, replace parent with child and make it black
    if (nodeIsRed || (nodeToBeRemoved->lchild && nodeToBeRemoved->lchild->color == Color::RED) ||
        (nodeToBeRemoved->rchild && nodeToBeRemoved->rchild->color == Color::RED)) {
        auto &child_pointer = getParentToChildPointer(nodeToBeRemoved);

        child_pointer = nodeToBeRemoved->lchild ?
            nodeToBeRemoved->lchild : nodeToBeRemoved->rchild;
        child_pointer->parent = nodeToBeRemoved->parent;
        child_pointer->color = Color::BLACK;

        goto out;
    }

#ifndef NDEBUG
    // At this point we consider nodeToBeRemoved has no children
    if (children_count == 1)
        throw std::runtime_error("nope");
#endif // !NDEBUG

    nodeToBeRemoved->color = Color::DBLACK;
    dbFix = nodeToBeRemoved;

    while (dbFix->color == Color::DBLACK && dbFix != root) {
        Node *sibling = getParentToSiblingPointer(dbFix);

        // If sibling is red, move it up, recolor and repeat
        if (sibling->color == Color::RED) {
            bool isLeftRotation = (sibling->parent->rchild == sibling);
            sibling->parent->color = Color::RED;
            if (isLeftRotation) {
                sibling->color = Color::BLACK;
                rotate_left(sibling);
            }
            else {
                sibling->color = Color::BLACK;
                rotate_right(sibling);
            }

            continue;
        }

        // Sibling is black \/

        // If sibling's children are black/null recolor sibling and repeat
        if ((!sibling->lchild || sibling->lchild->color == Color::BLACK) &&
            (!sibling->rchild || sibling->rchild->color == Color::BLACK)) {
            sibling->color = Color::RED;
            dbFix->color = Color::BLACK;

            dbFix = dbFix->parent;

            // If parent is red further balancing isnt needed
            if (dbFix->color == Color::RED) {
                dbFix->color = Color::BLACK;
                break;
            }

            dbFix->color = Color::DBLACK;
            continue;
        }

        // Fix dblack from other cases
        if (dbFix->color == Color::DBLACK)
            dbFix->color = Color::BLACK;

        // Swap node with parent if node's sibling is black/null
        // This is a special case preliminary to the actual tree rotation
        if (dbFix->parent->rchild == dbFix) {
            if (!sibling->lchild || sibling->lchild->color == Color::BLACK) {
                std::swap(sibling->color, sibling->rchild->color);
                rotate_left(sibling->rchild);
                sibling = getParentToSiblingPointer(dbFix);
            }
        }
        else if (!sibling->rchild || sibling->rchild->color == Color::BLACK) {
            std::swap(sibling->color, sibling->lchild->color);
            rotate_right(sibling->lchild);
            sibling = getParentToSiblingPointer(dbFix);
        }

        std::swap(sibling->color, sibling->parent->color);

        bool isLeftRotation = (sibling->parent->rchild == sibling);
        if (isLeftRotation) {
            sibling->rchild->color = Color::BLACK;
            rotate_left(sibling);
        }
        else {
            sibling->lchild->color = Color::BLACK;
            rotate_right(sibling);
        }

        // This case is always final, finish here
        break;
    }

    // Restore root coloring
    root->color = Color::BLACK;

    getParentToChildPointer(nodeToBeRemoved) = nullptr;

out:
    delete nodeToBeRemoved;

#ifndef NDEBUG
    counter--;
    if (counter != Node::counter)
        throw std::runtime_error("nope");

    if (root) {
        check_children(root);
        check_parent(root);
        check_coloring(root);
        check_depth(root);
    }
#endif // !NDEBUG

    return true;
}

template <typename T>
bool RBTree<T>::contains(const T &value) const
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
void RBTree<T>::print() const
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
        else if (kolejka.front()->color == Color::RED)
            std::cout << KRED << kolejka.front()->value << RST;
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
void RBTree<T>::check_children(Node *parent)
{
    T val = parent->value;

    if (parent->lchild) {
        if (val < parent->lchild->value)
            throw std::runtime_error("nope");
        check_children(parent->lchild);
    }
    if (parent->rchild) {
        if (val > parent->rchild->value)
            throw std::runtime_error("nope");
        check_children(parent->rchild);
    }
}

template <typename T>
void RBTree<T>::check_parent(Node *parent)
{
    if (parent == root)
        if (root->parent)
            throw std::runtime_error("nope");

    if (parent->lchild) {
        if (parent->lchild->parent != parent)
            throw std::runtime_error("nope");
        check_parent(parent->lchild);
    }
    if (parent->rchild) {
        if (parent->rchild->parent != parent)
            throw std::runtime_error("nope");
        check_parent(parent->rchild);
    }
}

template <typename T>
void RBTree<T>::check_coloring(Node *parent)
{
    if (parent == root)
        if (root->color != Color::BLACK)
            throw std::runtime_error("nope");

    if (parent->color == Color::DBLACK)
        throw std::runtime_error("nope");

    // Parent can't be red if its children are red
    bool check_color = (parent->color == Color::RED);

    if (parent->lchild) {
        if (check_color && parent->lchild->color == Color::RED)
            throw std::runtime_error("nope");
        check_coloring(parent->lchild);
    }
    if (parent->rchild) {
        if (check_color && parent->rchild->color == Color::RED)
            throw std::runtime_error("nope");
        check_coloring(parent->rchild);
    }
}

template <typename T>
void RBTree<T>::check_depth(Node *parent)
{
    if (!root)
        return;

    int depth = 1;
    Node *node = root;

    while (node->lchild) {
        if (node->lchild->color == Color::BLACK)
            depth++;
        node = node->lchild;
    }

    __check_depth(root, depth, 0);
}

template <typename T>
void RBTree<T>::__check_depth(Node *parent, const int &depth, int current_depth)
{
    if (parent->color == Color::BLACK)
        current_depth++;

    if (parent->lchild) {
        __check_depth(parent->lchild, depth, current_depth);
    }
    if (parent->rchild) {
        __check_depth(parent->rchild, depth, current_depth);
    }

    if (!parent->lchild && !parent->rchild)
        if (current_depth != depth)
            throw std::runtime_error("nope");
}
#endif // NDEBUG