#pragma once

#include <list>

using namespace std;

namespace trackman {

template <class T> bool listContains(const list<T> &list, T element) {
    return find(list.begin(), list.end(), element) != list.end();
}

} // namespace trackman
