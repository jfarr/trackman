#pragma once

#include <list>

template <class T> bool listContains(const std::list<T> &list, T element) {
    return std::find(list.begin(), list.end(), element) != list.end();
}
