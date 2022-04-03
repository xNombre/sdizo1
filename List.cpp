#pragma once

#include <iostream>

#include "List.hpp"

template <typename T>
List<T>::~List()
{
    Node *node = head;

    while (node) {
        Node *next = node->next;

        delete node;
        node = next;
    }
}

template <typename T>
void List<T>::push_front(const T &val)
{
    Node *swap = head;

    head = new Node();
    head->val = val;
    head->next = swap;

    if (swap)
        swap->prev = head;
    else
        // If theres no head, tail is nullptr too
        tail = head;
}

template <typename T>
void List<T>::push_back(const T &val)
{
    Node *node = new Node();
    node->val = val;
    node->prev = tail;

    if (tail)
        tail->next = node;
    else
        // If theres not tail, head is nullptr too
        head = node;

    tail = node;
}

template <typename T>
void List<T>::insert(const T &val, Node *&at)
{
    if (!at)
        throw std::runtime_error("Node is empty!");

    Node *node = new Node();
    node->val = val;
    node->prev = at->prev;
    node->next = at;

    // at can be head
    if (at->prev)
        at->prev->next = node;
    else
        head = node;

    // at will always be tail because we insert
    // before at

    at->prev = node;
}

template <typename T>
bool List<T>::contains(const T &val) const
{
    Node *node = get_node(val);

    return node;
}

template <typename T>
bool List<T>::remove(Node *&node)
{
    if (!node)
        return false;

    if (node->prev)
        node->prev->next = node->next;
    else
        // node is head
        if (node->next)
            head = node->next;
        else
            head = nullptr;

    if (node->next)
        node->next->prev = node->prev;
    else
        // node is tail
        if (node->prev)
            tail = node->prev;
        else
            tail = nullptr;

    delete node;
    node = nullptr;

    return true;
}

template <typename T>
void List<T>::pop_front() volatile
{
    if (!head)
        return;

    if (!head->next) {
        // Head is the last node in the list
        delete head;

        tail = nullptr;
        head = nullptr;
        return;
    }

    Node *tmp = head;
    head = head->next;
    head->prev = nullptr;

    delete tmp;
}

template <typename T>
void List<T>::pop_back()
{
    if (!tail)
        return;

    if (!tail->prev) {
        // Tail is the last node in the list
        delete tail;

        tail = nullptr;
        head = nullptr;
        return;
    }

    Node *tmp = tail;
    tail = tail->prev;
    tail->next = nullptr;

    delete tmp;
}

template <typename T>
bool List<T>::remove(const T &val)
{
    auto node = get_node(val);
    return remove(node);
}

template <typename T>
auto List<T>::get_node(const T &val) const -> List<T>::Node *
{
    Node *node = head;
    while (node) {
        if (node->val == val)
            return node;
        node = node->next;
    }
    return nullptr;
}

template <typename T>
void List<T>::print() const
{
    Node *node = head;

    while (node) {
        std::cout << node->val << " ";

        node = node->next;
    }
    std::cout << std::endl;
}