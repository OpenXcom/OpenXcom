#ifndef ARESAME_H
#define ARESAME_H

#include <limits>
#include <cstdlib>

template <class _Tx>
inline bool AreSame(const _Tx& l, const _Tx& r) {
  return std::abs(l-r) <= std::numeric_limits<_Tx>::epsilon();
}

#endif