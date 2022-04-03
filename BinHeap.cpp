#pragma once

#include <iostream>

#include "BinHeap.hpp"

#define PARENT_OF(child) (((child)-1) / 2)
#define L_CHILD_OF(parent) (((parent)*2 + 1))
#define R_CHILD_OF(parent) (((parent)*2 + 2))

template <typename T>
bool BinHeap<T>::search(const T &value, size_t &index) const
{
    index = 0;

    while (index != data.size()) {
        if (data[index] == value)
            return true;
        index++;
    }

    return false;
}

template <typename T>
void BinHeap<T>::add(const T &value)
{
    size_t pos = data.size();

    data.resize(pos + 1);
    data[pos] = value;

    // Heapify upwards
    while (pos != 0) {
        if (data[pos] > data[PARENT_OF(pos)])
            std::swap(data[pos], data[PARENT_OF(pos)]);
        pos = PARENT_OF(pos);
    }

#ifndef NDEBUG
    check_max();
#endif // !NDEBUG
}

template <typename T>
bool BinHeap<T>::remove(const T &value)
{
    size_t max = data.size() - 1;
    size_t index;

    if (!search(value, index))
        return false;

    data[index] = data[max];
    data.resize(max);

    // Removed node is the last node in the heap
    if (index == max)
        return true;

    // It is possible that leaf node was removed and
    // replaced node will be larger than its parent
    // in this case upwards heapify is needed
    if (index > 0 && data[PARENT_OF(index)] < data[index]) {
        // Heapify upwards
        while (index != 0) {
            if (data[index] > data[PARENT_OF(index)])
                std::swap(data[index], data[PARENT_OF(index)]);
            index = PARENT_OF(index);
        }
    }
    else {
        // Heapify downwards
        while (1) {
            size_t child = L_CHILD_OF(index);
            size_t candidate = index;

            if (child < max && data[candidate] < data[child])
                candidate = child;

            child = R_CHILD_OF(index);
            if (child < max && data[candidate] < data[child])
                candidate = child;

            if (candidate != index) {
                std::swap(data[index], data[candidate]);
                index = candidate;
            }
            else
                break;
        }
    }

#ifndef NDEBUG
    check_max();
#endif // !NDEBUG

    return true;
}

template <typename T>
bool BinHeap<T>::contains(const T &value) const
{
    return data.contains(value);
}

template <typename T>
void BinHeap<T>::print() const
{
    std::size_t row = 0, pow = 1;

    for (std::size_t i = 0; i < data.size(); i++) {
        std::cout << data[i] << " ";

        if (i == row) {
            pow *= 2;
            row += pow;
            std::cout << std::endl;
        }
    }

    std::cout << std::endl;
}

#ifndef NDEBUG
#include <queue>

template <typename T>
void BinHeap<T>::check_max() const
{
    std::queue<size_t> checkQueue;
    checkQueue.push(0);

    while (!checkQueue.empty()) {
        if (L_CHILD_OF(checkQueue.front()) < data.size()) {
            if (data[L_CHILD_OF(checkQueue.front())] > data[checkQueue.front()])
                throw std::runtime_error("nope");
            checkQueue.push(L_CHILD_OF(checkQueue.front()));
        }
        if (R_CHILD_OF(checkQueue.front()) < data.size()) {
            if (data[R_CHILD_OF(checkQueue.front())] > data[checkQueue.front()])
                throw std::runtime_error("nope");
            checkQueue.push(R_CHILD_OF(checkQueue.front()));
        }
        checkQueue.pop();
    }
}
#endif // !NDEBUG