#pragma once

#include <iostream>

#include "Array.hpp"

using std::size_t;

template <typename T>
void Array<T>::clear()
{
    if (array_size == 0)
        return;
    std::fill(array, array + array_size, 0);
}

template <typename T>
Array<T>::Array(const size_t &size) : array_size(size)
{
    if (size == 0)
        return;

    array = new T[size];
}

template <typename T>
Array<T>::Array(const std::initializer_list<T> &list) : Array(list.size())
{
    size_t pos = 0;
    for (auto val : list)
        array[pos++] = val;
};

template <typename T>
Array<T>::~Array()
{
    delete[] array;
}

template <typename T>
T &Array<T>::operator[](const size_t &at)
{
    if (at >= array_size)
        throw std::out_of_range("Array index out of range");
    return array[at];
}

template <typename T>
const T &Array<T>::operator[](const size_t &at) const
{
    if (at >= array_size)
        throw std::out_of_range("Array index out of range");
    return array[at];
}

template <typename T>
void Array<T>::resize(const size_t &size)
{
    T *new_array = new T[size];

    std::copy(array, array + std::min(array_size, size), new_array);

    delete[] array;
    array = new_array;

    array_size = size;
}

template <typename T>
void Array<T>::insert(const T &val, const size_t &at)
{
    if (at >= array_size)
        throw std::out_of_range("Inserting element outside of array!");

    size_t new_size = array_size + 1;
    T *new_array = new T[new_size];

    std::copy(array, array + at, new_array);
    std::copy(array + at, array + array_size, new_array + at + 1);

    new_array[at] = val;

    delete[] array;
    array = new_array;

    array_size = new_size;
}

template <typename T>
void Array<T>::push_back(const T &val)
{
    resize(array_size + 1);
    array[array_size - 1] = val;
}

template <typename T>
void Array<T>::push_front(const T &val)
{
    size_t new_size = array_size + 1;
    T *new_array = new T[new_size];

    std::copy(array, array + array_size, new_array + 1);

    new_array[0] = val;

    delete[] array;

    array = new_array;
    array_size = new_size;
}

template <typename T>
void Array<T>::pop_back()
{
    resize(array_size - 1);
}

template <typename T>
void Array<T>::pop_front()
{
    size_t new_size = array_size - 1;
    T *new_array = new T[new_size];

    std::copy(array + 1, array + array_size, new_array);

    delete[] array;

    array = new_array;
    array_size = new_size;
}

template <typename T>
const size_t &Array<T>::size() const
{
    return array_size;
}

template <typename T>
bool Array<T>::contains(const T &val) const
{
    for (int i = 0; i < array_size; i++) {
        if (array[i] == val)
            return true;
    }
    return false;
}

template <typename T>
bool Array<T>::remove(const T &val)
{
    size_t i = 0;
    for (; i < array_size; i++) {
        if (array[i] == val)
            break;
    }

    if (i == array_size)
        return false;

    size_t new_size = array_size - 1;
    T *new_array = new T[new_size];

    std::copy(array, array + i, new_array);
    std::copy(array + i + 1, array + array_size, new_array + i);

    delete[] array;
    array = new_array;

    array_size = new_size;

    return true;
}

template <typename T>
void Array<T>::print() const
{
    for (size_t i = 0; i < array_size; i++) {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;
}