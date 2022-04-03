#pragma once

#include "Array.hpp"

template<typename T>
class BinHeap {
    Array<T> data;

    bool search(const T &value, size_t &index) const;

#ifndef NDEBUG
    void check_max() const;
#endif // NDEBUG

public:
    void add(const T &value);
    bool remove(const T &value);
    bool contains(const T &value) const;
    void print() const;
};

// For template explicit instantiations
#include "BinHeap.cpp"