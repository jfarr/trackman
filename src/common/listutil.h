#pragma once

#include <list>

template <class T> bool listContains(T element, const std::list<T> &lis) {
    return std::find(lis.begin(), lis.end(), element) != lis.end();
}
