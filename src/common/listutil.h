#pragma once

#include <list>

template <class T> bool listContains(T element, const std::list<T> &list) {
    return std::find(list.begin(), list.end(), element) != list.end();
}
